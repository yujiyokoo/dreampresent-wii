class Dreampresent
  def initialize(dc_kos)
    @dc_kos = dc_kos
  end

  def start
    puts "Dreampresent: starting"
    # @dc_kos.pvr_initialise() # init is done in main for Wii
    Presentation.new(@dc_kos,
      PageData.new(@dc_kos).all
    ).run
  end
end
