class ShisaJump
  attr_accessor :dream_present

  def initialize(dream_present)
    @dream_present = dream_present
  end

  def start
    puts "ShisaJump started."
    SHISA1_X = 300
    GROUND_Y = 400
    INITIAL_SHISA2_SPEED = 8
    SHISA_HEIGHT = 32
    SHISA_WIDTH = 32
    while true do
      shisa1_y_vel = 0
      shisa1_y = GROUND_Y
      shisa2_x = 640
      previous_btns = dream_present.get_button_state
      dream_present.blank_screen
      dream_present.render_screen_and_wait
      current_shisa2_speed = INITIAL_SHISA2_SPEED

      while true do
        shisa2_x -= current_shisa2_speed
        if shisa2_x < 0
          current_shisa2_speed = (current_shisa2_speed * 15 / 10).to_i
          current_shisa2_speed = 128 if current_shisa2_speed > 128
          shisa2_x = 640
        end
        current_btns = dream_present.get_button_state
        if jumping?(previous_btns, current_btns) && can_jump?(shisa1_y)
          shisa1_y_vel = -24
          dream_present.play_jump_sound
        else
          shisa1_y_vel += 2
        end
        shisa1_y += shisa1_y_vel
        if shisa1_y >= GROUND_Y
          shisa1_y = GROUND_Y
          shisa1_y_vel = 0
        end

        dream_present.blank_screen
        dream_present.render_png("shisa2_flopped_png", shisa2_x, GROUND_Y)
        dream_present.render_png("shisa1_png", SHISA1_X, shisa1_y)
        dream_present.push_obj_buffer(PositionedPng.new("shisa2_flopped_png", shisa2_x, GROUND_Y))
        dream_present.push_obj_buffer(PositionedPng.new("shisa1_png", SHISA1_X, shisa1_y))
        dream_present.render_screen_and_wait

        if collided?(shisa1_y, shisa2_x, current_shisa2_speed)
          18.times { dream_present.wait_vbl }
          game_over
          shisa1_y = GROUND_Y
          shisa1_y_vel = -8
          shisa2_x = 640
          current_shisa2_speed = INITIAL_SHISA2_SPEED
        end

        dream_present.clear_obj_buffer
        previous_btns = current_btns
      end
    end
  end

  def collided?(shisa1_y, shisa2_x, current_shisa2_speed)
    (shisa1_y + SHISA_HEIGHT) > GROUND_Y && # <- Verticall check
      (shisa2_x > SHISA1_X - SHISA_WIDTH - (current_shisa2_speed / 2)) && (shisa2_x < SHISA1_X + SHISA_WIDTH) # <- Horizontal check
  end

  def game_over
    i = 0
    y_pos = GROUND_Y
    dream_present.play_hit_sound
    while i < 30 do
      y_pos += 8
      dream_present.render_png("shisa1_png", SHISA1_X, y_pos)
      dream_present.push_obj_buffer(PositionedPng.new("shisa1_png", SHISA1_X, y_pos))
      dream_present.render_screen_and_wait
      dream_present.clear_obj_buffer
      i += 1
    end
  end

  def can_jump?(y_pos)
    y_pos >= GROUND_Y
  end

  def jumping?(previous_btns, current_btns)
    remote_swing = dream_present.get_remote_state
    (!dream_present.btn_a?(previous_btns) && dream_present.btn_a?(current_btns)) || remote_swing == 1
  end
end
