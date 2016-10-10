require "../build/release/bfruby"

RUNS=10
OBFUSCATIONS = {:exact => 0, :low => 10, :medium => 20, :high => 25}

def run_benchmark(filename, query)
  res = {}

  res[:index] = (1..RUNS).map do
    GC.start
    t = BFTree.new
    t.index_file filename
    t.operation_time
  end.inject(0.0) {|sum, elem| sum + elem} / RUNS
  GC.start

  tree = BFTree.new
  tree.index_file filename

  OBFUSCATIONS.each do |k,v|
    res[k] = (1..RUNS).map do
      tree.search query, v
      tree.operation_time
    end.inject(0.0) {|sum, elem| sum + elem} / RUNS
  end
  return res
end

benchmarks = [
  ["../chromosomes/chr1.fa", "TTGCTACTTCATGCACTAAAATTAAAACCAACTGTAATCAGAGGGTGACT"],

  ["../chromosomes/chr10.fa", "taagttaattttcagccgggtgccaagaccattaaatgggaaaagaatag"],

  ["../chromosomes/chr21.fa", "tcttgagatgcgaatattattttgaattattccagtgggcccagtgtaat"]
]

benchmarks.each do |benchmark|
  filename, query = benchmark
  results = run_benchmark(filename, query)
  puts filename
  results.each do |k,v|
    puts "#{k}\t: #{v}"
  end
  puts "=" * 80
end



