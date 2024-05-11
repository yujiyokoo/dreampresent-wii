class Dreampresent
  attr_reader :dc_kos, :parser

  def initialize(dc_kos, parser)
    @dc_kos = dc_kos
    @parser = parser
  end

  def start
    puts "Dreampresent: starting"
    # @dc_kos.pvr_initialise() # init is done in main for Wii
    Presentation.new(
      dc_kos,
      PageData.new(dc_kos, parser).all
    ).run
  end
end
