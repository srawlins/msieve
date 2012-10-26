require File.join(File.dirname(__FILE__), 'test_helper')

class TC_Version < Test::Unit::TestCase
  def setup
    
  end

  def test_version
    assert_equal("0.1.0", Msieve::VERSION,  "msieve gem version should be correct")
    assert_equal("1.48",  Msieve::MSIEVE_VERSION,  "msieve version should be correct")
  end
end
