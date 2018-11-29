require 'net/http'
class AiDiscern
  def self.scan(url)
    uri = URI("http://localhost:8081/readimage?url=#{url}")
    resp = ::Net::HTTP.get(uri) 
    puts resp
    data = JSON.parse(resp)
    data
  end
end