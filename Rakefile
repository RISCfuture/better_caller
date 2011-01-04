require 'rubygems'
require 'bundler'
begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end
require 'rake'

require 'jeweler'
Jeweler::Tasks.new do |gem|
  # gem is a Gem::Specification... see http://docs.rubygems.org/read/chapter/20 for more options
  gem.name = "better_caller"
  gem.homepage = "http://github.com/RISCfuture/better_caller"
  gem.license = "MIT"
  gem.summary = %Q{Symbolic call stack with bindings}
  gem.description = %Q{A more programmer-friendly call stack complete with bindings for each level: no more string parsing\!}
  gem.email = "git@timothymorgan.info"
  gem.authors = ["Tim Morgan"]
  gem.extensions = [ 'ext/extconf.rb' ]
end
Jeweler::RubygemsDotOrgTasks.new

require 'yard'
YARD::Rake::YardocTask.new('doc') do |doc|
  doc.options << "-m" << "textile"
  doc.options << "--protected"
  doc.options << "--no-private"
  doc.options << "-r" << "README.textile"
  doc.options << "-o" << "doc"
  doc.options << "--title" << "better_caller Documentation".inspect
  
  doc.files = [ 'lib/**/*', 'README.textile' ]
end
