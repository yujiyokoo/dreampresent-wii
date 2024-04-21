require_relative "test_helper"

require 'page_data'

describe DrawLine do
  describe "#render" do
    it "calls draw_horizontal_line if :horizontal given" do
      dc_kos = FakeDcKos.new
      draw_line = DrawLine.new(dc_kos, :horizontal, 1, 0, 10, 0, 0, 0, 0)
      draw_line.render
      assert_equal :draw_horizontal_line, dc_kos.calls[0][0]
    end

    it "calls draw_vertical_line if :vertical given" do
      dc_kos = FakeDcKos.new
      draw_line = DrawLine.new(dc_kos, :vertical, 1, 0, 10, 0, 0, 0, 0)
      draw_line.render
      assert_equal :draw_vertical_line, dc_kos.calls[0][0]
    end
  end
end

describe PageData do
  describe "#all" do
    it "parses content string and return the result" do
      assert_equal "parsed content string", PageData.new(FakeDcKos.new, FakeParser.new).all
    end
  end
end