require 'rubygems'
require 'sinatra'
require 'haml'
require '../build/release/bfruby'

tree = BFTree.new
tree.index_file "../chromosomes/chr17.fa"
puts "Indexing took #{tree.operation_time} seconds."

get '/' do
  @query = ""

  haml :search
end

post '/' do
  @query   = params[:query]
  @matches = tree.search @query
  @time    = tree.operation_time

  haml :search
end

__END__
@@ search
%html
  %h1 Bloom filter search demonstration
  %form{:action => "/", :method => "post"}
    Query:
    %input{:type => "text", :name => "query", :value => @query}
    %input{:type => "submit", :value => "search"}
  - if @matches
    %p
      We found #{@matches.length} matches in #{@time} seconds.
      %table
        %tr
          %th match
          %th position
        - @matches.each do |match|
          %tr
            %td= match["match"]
            %td= match["position"]
