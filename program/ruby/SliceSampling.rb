#!/usr/bin/ruby

################################################################################
# default
################################################################################
@ifile = "hoge.cnf"
@ofile = "hoge.out"
@m = 1000
@s = 1000
@p = 0.5
@r = 0.5
@t = 0.1

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
  opts.on("-f [Input file]"){ |f|
    @ifile = f
  }
  opts.on("-o [Output file]"){ |f|
    @ofile = f
  }
  opts.on("-m [# Max walk (default 1000)]"){ |f|
    @m = f.to_i
  }
  opts.on("-s [# Samples (default 1000)]"){ |f|
    @s = f.to_i
  }
  opts.on("-p [Prob. of WalkSat vs SA (default 0.5)]"){ |f|
    @p = f.to_f
  }
  opts.on("-r [Prob. of RandomWalk vs GreedyWalk (default 0.5)]"){ |f|
    @r = f.to_f
  }
  opts.on("-t [Temperature (default 0.1)]"){ |f|
    @t = f.to_f
  }
  # parse
  opts.parse!(ARGV)
}


################################################################################
# Sat problem
################################################################################
class MySatProblem
  #### new ####
  def initialize
    @bady = nil
    @nvar = 0
    @ncnj = 0
    @cnf = nil
    @prb = nil
    @fix = nil
  end

  #### read ####
  def read(file)
    @body = file.gsub(/\..*/,"")
    cnf_file = sprintf("%s.cnf",@body)
    prb_file = sprintf("%s.prb",@body)

    #### Read cnf file ####
    f = open(cnf_file,"r")

    # skip comment
    while (line = f.gets).index(/^c/) != nil
    end

    # get number of variables and conjuncts
    l = line.split(" ")
    @nvar = l[2].to_i
    @ncnj = l[3].to_i
    @cnf = Array.new(@ncnj)
    @prb = Array.new(@nvar)

    # read conjuncts
    for i in 0..@ncnj-1
      line = f.gets
      l = line.split(" ")
      l.uniq!
      @cnf[i] = Array.new(l.size-1)
      for j in 0..l.size-2
        @cnf[i][j] = l[j].to_i
      end
    end
    f.close

    #### Read prb file ####
    if File.exist?(prb_file) == true
      f = open(prb_file,"r")
      i = 0
      while line = f.gets
        @prb[i] = line.to_f
        i += 1
      end
      f.close
    else
      for i in 0..@nvar-1
        @prb[i] = rand
      end
    end

    #### reset fixed value ####
    @fix = Array.new(@nvar)
    for i in 0..@nvar-1
      @fix[i] = -1 # "-1" means unfixed
    end
  end

  #### fix a variable ####
  def fix_var(id,val)
    i = id - 1
    if @fix[i] != -1
      abort("Already fixed")
    else
      @fix[i] = val
      fix_cnf(id,val)
    end
  end

  #### fix variables in a cluase ####
  def fix_cnf(id,val)
    t = id * (-1)**(1-val)
    f = id * (-1)**val

    for i in 0..@ncnj-1
      cls = @cnf[i]

      # t in cls -> cls is true
      if cls.index(t) != nil
        @cnf[i] = []

      # f in cls -> f should be removed from cls
      elsif cls.index(f) != nil
        @cnf[i].delete(f)
      end
    end

    # delete nil
    @cnf.uniq!
    @cnf.delete([])
    @ncnj = @cnf.size
  end

  #### ####
  def delete_unit
    @cnf.sort!{ |a,b| a.size <=> b.size}

    #### are there unit clauses? ####
    if @cnf.size == 0 || @cnf[0].size > 1
      return true
    else
      #### there is a unit clause ####
      i = @cnf[0][0]
      id = i.abs
      if i < 0
        val = 0
      else
        val = 1
      end
      fix_var(id,val)

      return delete_unit
    end
  end

  #### push ####
  def push(cls)
    # error check
    cls.each do |atom|
      id = atom.abs
      if id < 1 || id > @nvar
        abort("Error: invalid id")
      end
    end

    @cnf.push(cls)
    @ncnj += 1
  end

  #### clone ####
  def clone
    sat = MySatProblem.new
    sat.nvar = @nvar
    sat.ncnj = @ncnj
    sat.cnf = Array.new(@ncnj)
    for i in 0..@ncnj-1
      sat.cnf[i] = @cnf[i].clone
    end
    sat.prb = @prb.clone
    sat.fix = @fix.clone

    return sat
  end

  #### get_random_mdl ####
  def get_random_mdl
    mdl = Array.new(@nvar)
    for i in 0..@nvar-1
      if @fix[i] == -1
        mdl[i] = rand.to_i
      else
        mdl[i] = @fix[i]
      end
    end
    return mdl
  end

  #### get_i ###
  def get_i(i)
    return @cnf[i]
  end

  #### get_ij ####
  def get_ij(i,j)
    cnj = get_i(i)
    return cnj[j]
  end

  #### get prb ####
  def get_prb(mdl)
    p = 1.0
    for i in 0..@nvar-1
      if mdl[i] == 1
        p *= @prb[i]
      else
        p *= 1 - @prb[i]
      end
    end
    return p
  end

  #### get weight ####
  def get_weight(mdl)
    w = 1.0
    for i in 0..@ncnj-1
      if is_sat_cnj(mdl,i) == true
        w *= get_weight_cnj(i)
      else
        return 0
      end
    end
    return w
  end

  #### get weight cnj ####
  def get_weight_cnj(i)
    w = 0.0
    @cnf[i].each do |atom|
      id = atom.abs - 1
      if atom < 0
        w += (1-@prb[id])
      else
        w += @prb[id]
      end
    end
    return w
  end

  #### get number of sat clauses ####
  def get_nsc(mdl)
    n = 0
    for i in 0..@ncnj-1
      if is_sat_cnj(mdl,i) == true
        n += 1
      end
    end
    return n
  end

  #### is sat ####
  def is_sat(mdl)
    for i in 0..@ncnj-1
      if is_sat_cnj(mdl,i) == false
        return false
      end
    end
    return true
  end

  #### is sat cnj ####
  def is_sat_cnj(mdl,i)
    @cnf[i].each do |atom|
      id = atom.abs - 1
      if (atom < 0 && mdl[id].to_i == 0) || (atom > 0 && mdl[id].to_i == 1)
        return true
      end
    end
    return false
  end

  #### show ####
  def show
    p @nvar
    p @ncnj

    #### weights ####
    for i in 0..@nvar-1
      printf("%3d %f\n",i,@prb[i])
    end

    #### weighted clauses
    for i in 0..@ncnj-1
      cnj = @cnf[i]
      w = get_weight_cnj(i)
      printf("[%s] %f\n",cnj.join(","),w)
    end
  end

  #### getter ####
  attr_accessor :nvar, :ncnj, :cnf, :prb, :fix
end


################################################################################
# SampleSat = WalkSat + SA
################################################################################
class MySampleSat
  def initialize
    @sat = nil # the sat proble which should be solved
    @mdl = 0
    @stp = 0
    @m = 1000
    @p = 0.5   # prob WalkSat vs SA
    @r = 0.5   # prob Random Walk vs Greedy Walk
    @t = 0.1   # T=0.1
  end

  #### sovlve the sat problem ####
  def solve(sat)
    #### init ####
    @sat = sat
    @mdl = sat.get_random_mdl
    @stp = 0

    #### empty ####
    if @sat.ncnj == 0
      return @mdl.clone
    end

    #### walk ####
    begin
      v = walk
    end while @stp < @m && v == false

    return @mdl.clone
  end

  #### walk ####
  def walk
    if rand < @p
      if rand < @r
        random_walk
      else
        greedy_walk
      end
    else
      sa_walk
    end
    @stp += 1

    return @sat.is_sat(@mdl)
  end

  #### random_walk ####
  def random_walk
    # get unsat conjuncts
    unsat_cnj = Array.new
    for i in 0..@sat.ncnj-1
      if @sat.is_sat_cnj(@mdl,i) == false
        unsat_cnj.push(i)
      end
    end

    # chose one from them
    i = rand(unsat_cnj.size)
    j = rand(@sat.get_i(i).size)
    id = @sat.get_ij(i,j).abs - 1
    @mdl[id] = 1 - @mdl[id]
  end

  #### greedy_walk ####
  def greedy_walk
    max_id = 0
    max_nsc = 0
    for i in 0..@sat.nvar-1
      if @sat.fix[i] == -1
        mdl = @mdl.clone
        mdl[i] = 1 - mdl[i]
        nsc = @sat.get_nsc(mdl)
        if nsc > max_nsc
          max_nsc = nsc
          max_id = i
        end
      end
    end
    @mdl[max_id] = 1 - @mdl[max_id]
  end

  #### sa_walk ####
  def sa_walk
    mdl = @mdl.clone

    # chose and flip
    begin
      i = rand(@sat.nvar)
    end while @sat.fix[i] != -1
    mdl[i] = 1 - mdl[i]

    nsc0 = @sat.get_nsc(@mdl)
    nsc1 = @sat.get_nsc(mdl)

    r = Math.exp(-get_temp)
    if nsc0 < nsc1 || rand < r
      @mdl[i] = 1 - @mdl[i]
    end
  end

  #### get temp ####
  def get_temp
    return @t
  end

  #### setter ####
  attr_accessor :m, :r, :p, :t
  attr_reader :stp, :mdl
end


################################################################################
# Slice sampling
################################################################################
class MySliceSampling
  #### new ####
  def initialize(sol)
    @x = nil # current model
    @u = nil # current u

    #### SampleSAT options ####
    @sol = sol
  end

  #### slice sapling ####
  def sample(sat,n)
    @sat = sat
    hst = Array.new

    # init satate
    @x = @sol.solve(@sat)
    @u = Array.new(@sat.nvar)
    for i in 1..n
      printf("%5d solved? %5s %10d steps\n",i,@sat.is_sat(@x),@sol.stp)
      sample_u
      sample_x
      hst.push(@x)
    end

    return hst
  end

  #### sample from p(u|x) ####
  def sample_u
    for i in 0..@sat.nvar-1
      # prob
      if @x[i] == 1
        p = @sat.prb[i]
      else
        p = 1 - @sat.prb[i]
      end

      # uniform
      @u[i] = rand * p
    end
  end

  #### sample from p(x|u) ####
  def sample_x
    sat = @sat.clone

    for i in 0..@sat.nvar-1
      # prob
      p1 = @sat.prb[i]
      p0 = 1 - @sat.prb[i]

      # chenge sat problem
      id = i+1
      if p0 < @u[i] && @u[i] < p1
        sat.push([id])
      elsif p1 < @u[i] && @u[i] < p0
        sat.push([-id])
      end
    end
    sat.delete_unit

    # sampling
    mdl = @sol.solve(sat)
    if sat.is_sat(mdl) == true
      @x = mdl
    end
  end
end


################################################################################
# main
################################################################################
#### read a sat problem ####
sat = MySatProblem.new
sat.read(@ifile)
frq_sat = Hash.new(0)
frq_slc = Hash.new(0)

########################################
# SampleSAT
########################################
sol = MySampleSat.new
sol.m = @m
sol.p = @p
sol.r = @r
sol.t = @t
printf("p = %f, r = %f, t = %f\n",@p,@r,@t)

#### sampling ####
p "SampleSAT"
for i in 1..@s
  mdl = sol.solve(sat)
  frq_sat[mdl] += 1

  #### result ####
  printf("%5d solved? %5s %10d steps\n",i,sat.is_sat(sol.mdl),sol.stp)
end


########################################
# Slice Sampling
########################################
ssr = MySliceSampling.new(sol)

#### Sampling
p "Slice Sampling"
hst = ssr.sample(sat,@s)
hst.each do |mdl|
  frq_slc[mdl] += 1
end

########################################
# Results
########################################
#### all models found by samplings
mdls = frq_sat.keys + frq_slc.keys
mdls.uniq!

#### approximated normalized term
z = 0.0
mdls.each do |mdl|
  if sat.is_sat(mdl) == true
    z += sat.get_prb(mdl)
  end
end

#### print
f = open(@ofile,"w")
f.printf("p = %f, r = %f, t = %f\n",@p,@r,@t)

# models
printf("Model & probability                       SampleSAT  SliceSampling\n")
mdls.each do |mdl|
  # prob. of the model
  if sat.is_sat(mdl) == true
    p = sat.get_prb(mdl) / z
  else
    p = 0.0
  end

  # for stout
  printf("[%s](%f)",mdl.join(","),p)
  printf("%20d",frq_sat[mdl])
  printf("%15d\n",frq_slc[mdl])

  # for file
  f.printf("[%s] (%f)",mdl.join(","),sat.get_prb(mdl)/z)
  f.printf("%30d",frq_sat[mdl])
  f.printf("%15d\n",frq_slc[mdl])
end
f.close
