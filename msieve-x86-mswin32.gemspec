Gem::Specification.new do |s|
  s.name = "msieve"
  s.version = "0.1.0"
  s.authors = ["Sam Rawlins"]
  s.date = "2010-05-04"
  s.description = "msieve - providing Ruby bindings to the msieve library."
  s.email = ["sam.rawlins@gmail.com"]
  
  #s.extensions = ["ext/extconf.rb"]
  s.platform = Gem::Platform::CURRENT
  #s.has_rdoc = "yard"
  s.homepage = "http://github.com/srawlins/msieve"
  s.summary = "Provides Ruby bindings to the msieve library."

  s.required_ruby_version = '>= 1.8.6'
  s.requirements = ["msieve compiled and working properly."]
  s.require_paths = ["lib"]
  s.files  = Dir["ext/*.c"] + Dir["ext/*.h"] + ["ext/extconf.rb"]
  s.files += Dir["lib/msieve.rb"]
  s.files += ["ext/libgmp-10.dll"] + ["ext/libmsieve.a"] + ["ext/msieve.so"]
  s.files += Dir["test/*.rb"]
  s.files += ["README.markdown"]
end
