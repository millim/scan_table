class HomeController < ApplicationController


  def create
    uploaded_io = params[:picture_image]
    file_name = Time.now.strftime('%Y%m%d%H%M%S') + '%04d' % [rand(9999)] + uploaded_io.original_filename
    file_url  = Rails.root.join('uploads','ai', file_name)
    File.open(file_url, 'wb') do |file|
      file.write(uploaded_io.read)
    end
    data = AiDiscern.scan(file_url)
    @momo = data
    render :index
  end


end