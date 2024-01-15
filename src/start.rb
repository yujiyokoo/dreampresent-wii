begin
  DreamPresentWii.print_msg 'Starting presentation.'
  Dreampresent.new(DcKosRb.new(DreamPresentWii)).start
rescue => ex
  # Note backtrace is only available when you pass -g to mrbc
  print "ERROR! "
  p ex
  puts ex.backtrace
  raise ex
end
