require File.join(File.dirname(__FILE__), 'test_helper')

class TC_Initiate < Test::Unit::TestCase
  def setup
    @sample_fixnums = [  1,  2,  3,  4,  5,  8,  9, 15, 16, 17, 31, 32, 33,
                        2**10-1, 2**10, 2*3*4*5*6*7, 10**6, 2**20]
    @sample_strings = ["1",  "2",  "3",  "4",  "5",  "8",  "9", "15",
                       "16", "17", "31", "32", "33", "2**10-1", "2**10",
                       "2*3*4*5*6*7", "10**6", "2**20"]
  end

  def test_initiate
    (@sample_fixnums + @sample_strings).each do |i|
      m = Msieve.new(i)
      assert_equal(i.to_s, m.input, "Msieve.new(#{i.inspect}) should initiate ok.")
    end
  end
end
