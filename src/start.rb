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

class Time
  def self.now
    Time.at(DreamPresentWii.int_time)
  end
end

begin
  # DreamPresentWii.print_msg 'Starting presentation.'
  DreamPresentWii.print_msg "test message from print_msg"
  puts "test message from puts"
  p "test message from p"
  Dreampresent.new(DcKosRb.new(DreamPresentWii)).start
rescue => ex
  # Note backtrace is only available when you pass -g to mrbc
  DreamPresentWii.print_msg ex.inspect + "\n" + ex.backtrace.join("\n")

  # old method
  # print "ERROR! "
  # p ex
  # puts ex.backtrace
  raise ex
end
