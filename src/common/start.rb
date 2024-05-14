# define a new version of puts and p so they show up in terminal
module Kernel
  alias_method :original_puts, :puts
  def puts(*args)
    original_puts(*args)
    original_puts "\r"
  end

  alias_method :original_p, :p
  def p(*args)
    original_p(*args)
    original_puts "\r"
  end
end

class << Time
  alias_method :original_now, :now
end

class Time
  def self.now
    Time.at(DreamPresentRb.new(DreamPresent).int_time)
  end
end

begin
  # puts 'Starting presentation.'
  dream_present = DreamPresentRb.new(DreamPresent)
  Dreampresent.new(dream_present, Parser.new).start
  ShisaJump.new(dream_present).start
rescue => ex
  # Note backtrace is only available when you pass -g to mrbc
  puts ex.inspect + "\n" + ex.backtrace.join("\n")

  # old method
  # print "ERROR! "
  # p ex
  # puts ex.backtrace
  raise ex
end
