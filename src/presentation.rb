class Presentation
  include Commands

  def initialize(dc_kos, pages)
    @dc_kos, @pages = dc_kos, pages
    @start_time = Time.now
  end

  def clear_page
		(0..32).each { |row|
			(0..24).each { |col|
				draw_black_square(row, col, false)
			}
		}
    @dc_kos::reset_print_pos
	end
  LEFT_SPACE_PX=0
  TOP_SPACE_PX=0

  def draw_square(x, y, r, g, b, ignore_boundary)
    if (x >= 0 && x < 32 && y >= 0 && y < 24) || ignore_boundary
      #puts "x: #{ (x*10+LEFT_SPACE_PX) }, y: #{ y*20+TOP_SPACE_PX } "
      @dc_kos::draw20x20_640(x*10+LEFT_SPACE_PX, y*20+TOP_SPACE_PX, r, g, b)
    end
  end

  def draw_black_square(x, y, ignore_boundary)
    draw_square(x, y, 0, 255, 0, ignore_boundary)
  end

  def run
    idx = 0
    time_adjustment = 0
    @dc_kos::init_controller_buffer
    @dc_kos::start_controller_reader
    while(idx < @pages.length)
      clear_page
      input = @pages[idx].show(@dc_kos,
        PresentationState.new(idx, time_adjustment),
        @start_time
      )

      case
      when input == NEXT_PAGE
        idx += 1
      when input == PREVIOUS_PAGE
        idx -= 1
      when input == QUIT
        idx += @pages.length
      when input == SWITCH_VIDEO_MODE
        @dc_kos.next_video_mode
      when input == RESET_TIMER
        @start_time = Time.now
        idx += 1
      when input == FWD
        time_adjustment += 300
        time_adjustment = (40 * 60) if time_adjustment > (40 * 60)
      when input == REW
        time_adjustment -= 300
        time_adjustment = 0 if time_adjustment < 0
      end
      #puts "- - - - current idx = #{idx}"
      idx = 0 if idx < 0 # do not wrap around backwards
    end
  end
end
