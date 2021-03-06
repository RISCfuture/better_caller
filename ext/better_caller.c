#include "ruby.h"
#include "vm_core.h"

#define ENV_IN_HEAP_P(th, env)  \
  (!((th)->stack < (env) && (env) < ((th)->stack + (th)->stack_size)))

static VALUE binding_from_cfp(rb_thread_t *th, rb_control_frame_t *cfp) {
	if (cfp == 0) return Qnil;
	
	VALUE *penvptr = GC_GUARDED_PTR_REF(*(cfp->dfp));
	rb_control_frame_t *pcfp = VM_FRAME_TYPE(cfp) == VM_FRAME_MAGIC_FINISH ? RUBY_VM_PREVIOUS_CONTROL_FRAME(cfp) : cfp;
	if (!ENV_IN_HEAP_P(th, penvptr)) {
		while (pcfp->dfp != penvptr) {
			pcfp++;
			if (pcfp->dfp == 0) return Qnil;
		}
	}
	
	VALUE bindval = rb_binding_new();
	rb_binding_t *bind;
	
	GetBindingPtr(bindval, bind);
	bind->env = rb_vm_make_env_object(th, cfp);
	return bindval;
}

static VALUE backtrace_each(rb_thread_t *th, const rb_control_frame_t *limit_cfp, const rb_control_frame_t *cfp, const char *file, int line_no, VALUE ary) {
	VALUE elem, locals;
	int i;

	while (cfp > limit_cfp) {
		elem = 0;
		locals = 0;
		if (cfp->iseq != 0) {
			if (cfp->pc != 0) {
				rb_iseq_t *iseq = cfp->iseq;

				elem = rb_ary_new();
				rb_ary_push(elem, iseq->filename);
				rb_ary_push(elem, INT2FIX(rb_vm_get_sourceline(cfp)));
				rb_ary_push(elem, rb_str_intern(iseq->name));
				rb_ary_push(elem, binding_from_cfp(th, cfp));
				rb_ary_push(ary, elem);
			}
		}
		else if (RUBYVM_CFUNC_FRAME_P(cfp)) {
			elem = rb_ary_new();
			rb_ary_push(elem, rb_str_new2(file));
			rb_ary_push(elem, INT2FIX(line_no));
			rb_ary_push(elem, ID2SYM(cfp->me->def ? cfp->me->def->original_id : cfp->me->called_id));
			rb_ary_push(elem, Qnil);
			rb_ary_push(ary, elem);
		}
		cfp = RUBY_VM_NEXT_CONTROL_FRAME(cfp);
	}
	return rb_ary_reverse(ary);
}

static inline VALUE backtrace(rb_thread_t *th, int lev) {
	VALUE ary;
	const rb_control_frame_t *cfp = th->cfp;
	const rb_control_frame_t *top_of_cfp = (void *)(th->stack + th->stack_size);
	top_of_cfp -= 2;

	if (lev < 0) ary = rb_ary_new();
	else {
		while (lev-- >= 0) {
			cfp++;
			if (cfp >= top_of_cfp) return Qnil;
		}
		ary = rb_ary_new();
	}

	ary = backtrace_each(th, RUBY_VM_NEXT_CONTROL_FRAME(cfp), top_of_cfp, RSTRING_PTR(th->vm->progname), 0, ary);
	return ary;
}

static VALUE better_caller(int argc, VALUE *argv) {
	VALUE level;
	int lev;

	rb_scan_args(argc, argv, "01", &level);

	if (NIL_P(level)) lev = 1;
	else lev = NUM2INT(level);
	if (lev < 0) rb_raise(rb_eArgError, "negative level (%d)", lev);

	return backtrace(GET_THREAD(), lev);
}

static VALUE exception_better_backtrace(VALUE self) {
	return rb_iv_get(self, "@better_backtrace");
}

static void process_event(rb_event_flag_t event, VALUE data, VALUE self, ID id, VALUE klass) {
 	switch (event) {
 		case RUBY_EVENT_RAISE:
			rb_funcall(rb_gv_get("$!"), rb_intern("set_better_backtrace"), 1, better_caller(0, 0));
 			break;
	}
}

void Init_better_caller() {
	rb_define_method(rb_eException, "better_backtrace", exception_better_backtrace, 0);
	rb_define_global_function("better_caller", better_caller, -1);
	rb_add_event_hook(process_event, RUBY_EVENT_RAISE, Qnil);
}
