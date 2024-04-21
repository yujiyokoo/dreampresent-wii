require "minitest/autorun"

$:.push("src")
$:.push("test")

require 'platform_lib'

class FakePage
  include ::Commands

  def show(dc_kos, presentation_state, start_time)
    NEXT_PAGE
  end
end

class FakeDcKos
  attr_reader :calls

  def initialize
    @calls = []
  end

  def push_obj_buffer(obj)
    @obj = obj
  end

  def draw_horizontal_line(*args)
    @calls.push([:draw_horizontal_line, *args])
  end

  def draw_vertical_line(*args)
    @calls.push([:draw_vertical_line, *args])
  end

  def content_string
    "content string"
  end
end

class FakeParser
  def parse(str)
    "parsed " + str
  end
end