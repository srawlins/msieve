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
  
  def prime?
    if factors
      return (factors.size == 1 and factors[0].factor_type != :composite)
    else
      raise "Cannot test for primality. "+inspect+" has not been factored (use Msieve#factor!)."
    end
  end
end

begin; GMP::Z
  module GMP; class Z
    def factor
      Msieve.new(to_i).factor!
    end
    
    def prime?
      Msieve.new(to_i).factor!.size == 1
    end
  end; end
rescue NameError

end