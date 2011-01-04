require 'mkmf'

create_makefile 'better_caller'
require File.expand_path(File.dirname(__FILE__) + '/../lib/better_caller/extensions')
