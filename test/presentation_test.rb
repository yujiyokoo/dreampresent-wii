require_relative "test_helper"

require 'platform_lib'
require 'presentation'

describe Presentation do
  describe "#run" do
    it "goes to the last page" do
      presentation = Presentation.new(FakeDcKos.new, [FakePage.new, FakePage.new])
      presentation.run
      assert_equal 2, presentation.instance_variable_get(:@idx)
    end
  end

  describe "#navigate" do
    let(:presentation) {
      Presentation.new(FakeDcKos.new, [FakePage.new, FakePage.new])
    }

    it "goes to the next page for NEXT_PAGE" do
      presentation.navigate(Commands::NEXT_PAGE)
      assert_equal 1, presentation.instance_variable_get(:@idx)
    end

    it "goes to the previous page for PREVIOUS_PAGE" do
      presentation.instance_variable_set(:@idx, 1)
      presentation.navigate(Commands::PREVIOUS_PAGE)
      assert_equal 0, presentation.instance_variable_get(:@idx)
    end

    it "goes to the last page for QUIT" do
      presentation.navigate(Commands::QUIT)
      assert_equal 2, presentation.instance_variable_get(:@idx)
    end

    it "creates a new start_time for RESET_TIMER" do
      past = Time.at(Time.now.to_i - 3)
      presentation.instance_variable_set(:@start_time, past)
      assert_equal past, presentation.instance_variable_get(:@start_time)
      presentation.navigate(Commands::RESET_TIMER)
      assert Time.at(past) < presentation.instance_variable_get(:@start_time)
    end

    it "sets time_adjustment for FWD" do
      presentation.navigate(Commands::FWD)
      assert_equal 300, presentation.instance_variable_get(:@time_adjustment)
    end

    it "sets time_adjustment to max for FWD if it goes past max" do
      presentation.instance_variable_set(:@time_adjustment, 40 * 59)
      presentation.navigate(Commands::FWD)
      assert_equal 40 * 60, presentation.instance_variable_get(:@time_adjustment)
    end

    it "sets time_adjustment for REW" do
      presentation.instance_variable_set(:@time_adjustment, 350)
      presentation.navigate(Commands::REW)
      assert_equal 50, presentation.instance_variable_get(:@time_adjustment)
    end

    it "sets time_adjustment 0 for REW instead of negative" do
      presentation.navigate(Commands::REW)
      assert_equal 0, presentation.instance_variable_get(:@time_adjustment)
    end
  end
end

