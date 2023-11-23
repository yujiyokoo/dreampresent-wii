begin
  # DreamPresentWii.print_msg 'Starting presentation.'
  Dreampresent.new(DcKosRb.new(DreamPresentWii)).start
rescue => ex
  #DreamPresentWii.print_msg ex.inspect
  # Note backtrace is only available when you pass -g to mrbc
  DreamPresentWii.print_msg ex.inspect + "\n" + ex.backtrace.join("\n")
  raise ex
end
