file = "result.txt"

################################################################################
# Arguments
################################################################################
require "optparse"
OptionParser.new { |opts|
  # options
  opts.on("-h","--help","Show this message") {
    puts opts
    exit
  }
  opts.on("-f [string]","File name"){ |f|
    file = f
  }
  # parse
  opts.parse!(ARGV)
}

################################################################################
# main
################################################################################
#kl_sa = 0.0
#kl_sl = 0.0

#sq_sa = 0.0
#sq_sl = 0.0

p file
f= open(file,"r")
while line = f.gets
  if line.index(/%%sample/) != nil
    # read a line
    l = line.gsub(/\n/,"").split(/,/)
    p = l[3].to_f
    p_sa = l[5].to_f
    p_sl = l[6].to_f

    # KL
    kl_sa += p * (Math::log(p) - Math::log(p_sa))
    kl_sl += p * (Math::log(p) - Math::log(p_sl))

    # squared error
    sq_sa += (p - p_sa)**2
    sq_sl += (p - p_sl)**2
  end
end
f.close

printf("KL%15.5e%15.5e%15.5e\n",kl_sa,kl_sl,kl_sl-kl_sa)
printf("SQ%15.5e%15.5e%15.5e\n",sq_sa,sq_sl,sq_sl-sq_sa)

