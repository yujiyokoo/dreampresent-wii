module Commands
  NEXT_PAGE = 1
  PREVIOUS_PAGE = -1
  QUIT = -2
  FWD = -3
  REW = -4
  SWITCH_VIDEO_MODE = -5
  RESET_TIMER = -6
end

# Ruby-level wrapper to DcKos
class DreamPresentRb
  include Commands

  def initialize(dc_kos)
    @dc_kos = dc_kos
    @obj_buffer = []
  end

  def self.colour_to_rgb(colour)
    case colour
    when 'red'
        [255, 0, 0]
    when 'magenta'
        [255, 0, 255]
    when 'ltblue'
        [135, 206, 250]
    when 'yellow'
        [255, 255, 0]
    when 'ltgreen'
        [144, 238, 144]
    when 'cyan'
      [0, 255, 255]
    when 'black'
      [0, 0, 0]
    else # unknown colours default to white
        [255, 255, 255]
    end
  end

  LINE_HEIGHT = 30
  # this understands '\n' as linebreak
  def draw_str(str, x, y, line_height = LINE_HEIGHT, colour, show_bg)
    rgb = self.class.colour_to_rgb(colour)
    bg_on =
      if ['true', 'yes'].include? show_bg
        1
      else
        0
      end

    str.split("\n").each_with_index { |line, idx|
      @dc_kos.draw_str(line, x, y + (line_height * idx+1), *rgb, bg_on)
    }
#    str.split("\n").each_with_index { |line, idx|
#      dc_kos = @dc_kos
#      @obj_buffer.push Object.new.tap { |o|
#        o.define_singleton_method(:render) do
#          puts "draw_str: #{line}"
#          dc_kos.draw_str(line, x, y + (line_height * idx+1), *rgb, bg_on)
#        end
#      }
#    }
  end

  # this is for 512x512 images
  def load_fullscreen_png(filepath)
    load_png(filepath, 1, 1, 640, 640)
  end

  # This and method_missing are for delegating functions not defined here to @dc_kos
  def respond_to?(method)
    if method == :respond_to?
      true
    else
      self.respond_to?(method) || @dc_kos.respond_to?(method)
    end
  end

  # This and respond_to? are for delegating functions not defined here to @dc_kos
  def method_missing(method, *args, &block)
    if @dc_kos.respond_to?(method)
      @dc_kos.send(method, *args, &block)
    else
      fail NoMethodError, "undefined method '#{method}'"
    end
  end

  def next_or_back(clear_screen_on_nav = true)
    previous_state = @dc_kos::get_button_state
    previous_fishing_swing_state = @dc_kos::get_fishing_controller_swing_state
    while true do
      button_state = @dc_kos::get_button_state
      fishing_swing_state = @dc_kos::get_fishing_controller_swing_state

      render_screen_and_wait

      # NOTE order is important here.

      return QUIT if quit_combination?(previous_state, button_state)

      return SWITCH_VIDEO_MODE if switch_video_mode_combination?(previous_state, button_state)

      # press STRAT or A to go forward (or swing the fishing controller)
      if start_or_a_pressed?(previous_state, button_state) || (fishing_swing_state > 200 && previous_fishing_swing_state <= 200)
#        clear_obj_buffer if clear_screen_on_nav
        play_test_sound
          return NEXT_PAGE
      end

      # press B to go back
      if b_pressed?(previous_state, button_state)
#        clear_obj_buffer if clear_screen_on_nav
        return PREVIOUS_PAGE
      end

      # left and right on dpad for skipping or rewinding the time indicator
      return FWD if right_pressed?(previous_state, button_state)
      return REW if left_pressed?(previous_state, button_state)

      previous_state = button_state
      previous_fishing_swing_state = fishing_swing_state
    end
  end

  # NOTE: This requires the source to be 512x512 png!
  # x, y are zero-based but note the pvr uses 1-based coordinates (hence the '+ 1')
  # NOTE: Before calling this, you need to initialise PVR like: dc_kos.pvr_initialise()
  # @dc_kos.show_512x512_png('/rd/dc_controller_orig400x362.png', 0, 0, 640, 480)
  def show_512x512_png(path, x, y, w, h)
    @dc_kos::load_png(path, x+1, y+1, x+w, y+h)
  end

  # press A + B + Start
  def quit_combination?(previous, current)
      (@dc_kos::btn_a?(current) && @dc_kos::btn_b?(current) && @dc_kos::btn_start?(current))
  end

  def switch_video_mode_combination?(previous, current)
    !(@dc_kos::dpad_down?(previous) && @dc_kos::btn_a?(previous)) &&
      (@dc_kos::dpad_down?(current) && @dc_kos::btn_a?(current))
  end

  def start_or_a_pressed?(previous, current)
    (!@dc_kos::btn_start?(previous) && @dc_kos::btn_start?(current)) ||
      (!@dc_kos::btn_a?(previous) && @dc_kos::btn_a?(current))
  end

  def b_pressed?(previous, current)
    !@dc_kos::btn_b?(previous) && @dc_kos::btn_b?(current)
  end

  def b_pressed?(previous, current)
    !@dc_kos::btn_b?(previous) && @dc_kos::btn_b?(current)
  end

  def right_pressed?(previous, current)
    !@dc_kos::dpad_right?(previous) && @dc_kos::dpad_right?(current)
  end

  def left_pressed?(previous, current)
    !@dc_kos::dpad_left?(previous) && @dc_kos::dpad_left?(current)
  end

  # added for compatibility
  def int_time
    Time.original_now.to_i
  end

  def clear_obj_buffer
    @obj_buffer = []
  end

  def push_obj_buffer(obj)
    puts "pushing obj #{obj}"
    @obj_buffer.push(obj)
  end

  def render_png(img_path, x, y)
    DreamPresentPng.new(img_path).render(x, y)
  end

  def render_screen_and_wait
    @dc_kos::wait_vbl
  end
end

class DreamPresentPng
  def initialize(name)
    # Wii version looks up, but DC version will just prepend the path
    # TODO: handle this properly... maybe unify the data/romdisk dirs first
    @name = if name[0..3] == "/rd/"
      name
    else
      "/rd/#{name}"
    end

    @name = if @name[-4..-1] == "_png"
      @name[0..-5] + '.png'
    else
      @name
    end
  end

  def render(x, y)
    DreamPresent::render_png(@name, x, y)
  end
end
