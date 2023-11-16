begin
  # DreamPresentWii.print_msg 'Starting presentation.'
  Dreampresent.new(DcKosRb.new(DreamPresentWii)).start
rescue => ex
  DreamPresentWii.print_msg "error"
  # Note backtrace is only available when you pass -g to mrbc
  # DreamPresentWii.print_msg ex.backtrace
  DreamPresentWii.print_msg ex.inspect
  raise ex
end
