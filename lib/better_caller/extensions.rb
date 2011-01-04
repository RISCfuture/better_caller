# @private
class Exception
  # @private
  def set_better_backtrace(bt)
    @better_backtrace = bt.collect do |(file, line, meth, bind)|
      vars = {
        local_variables: eval("local_variables.inject({}) { |hsh, var| hsh[var] = eval(var) ; hsh }", bind),
        instance_variables: eval("instance_variables.inject({}) { |hsh, var| hsh[var] = eval(var) ; hsh }", bind),
        global_variables: eval("global_variables.inject({}) { |hsh, var| hsh[var] = eval(var) ; hsh }", bind),
      }
      [ file, line, meth, vars]
    end
  end
end
