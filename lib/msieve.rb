# Thank you, Nokogiri

require 'rbconfig'

ENV['PATH'] = [File.expand_path(
  File.join(File.dirname(__FILE__), "..", "ext")
), ENV['PATH']].compact.join(';') if RbConfig::CONFIG['host_os'] =~ /(mswin|mingw|mingw32)/i

require File.dirname(__FILE__) + '/../ext/msieve'

class Msieve
  def self.clear_log(log_name = "msieve.log")
    if File.exist? log_name
      File.delete log_name
    end
    puts "Cleared! (but not really)."
  end
end