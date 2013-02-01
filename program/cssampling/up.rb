#!/usr/bin/ruby

################################################################################
# default
################################################################################
@body = "hoge"
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
  opts.on("-b [string]"){ |f|
    @body = f
  }
  # parse
  opts.parse!(ARGV)
}

################################################################################
# Class
################################################################################
class PreCnf
  def initialize(body)
    @body = body
    @count = 0
    @name = Hash.new(nil)
    @ucs = 0
    get_file
    show
  end

  def get_file
    idx = 0
    file = sprintf("%s.cnf",@body)
    if File::exist?(file) == true
      f = open(file,"r")
      while line = f.gets
        arg = get_args(line)
        if arg[0] =~ /p/
          @cnf = Array.new(arg[3].to_i){[]}
          @atm = Array.new
          @nv = arg[2]
          @nc = arg[3]
        elsif arg[0] =~ /[1-9]/
          for i in 0..arg.size-2
            @cnf[idx][i] = arg[i].to_i
          end
          if arg[1] == "0"
            @atm.push(arg[0].to_i)
            @ucs += 1
          end
          idx += 1
        end
      end
      f.close
    elsif
      exit
    end
  end

  def get_body
    return @body
  end

  def get_ucs
    return @ucs
  end

  def get_nc
    return @nc
  end

  def get_nv
    return @nv
  end

  def get_size
    return @cnf.size
  end

  def get_cnfi(i)
    return @cnf[i]
  end

  def to_a(c)
    ary = Array.new
    for i in 0..@name.size-1
      if @name.keys[i].include?(c)
        x = @name.keys[i]
        ary.push([x])
      end
    end
    return ary
  end

  def get_args(line)
    return line.chop.gsub(/ /,",").split(",")
  end

  def p_atom
    printf("rule:%2d , atom:%d\n",@count,@name.size)
    @count += 1
  end

  def show
    printf("## CNF ##\n")
    for j in 0..@cnf.size-1
      for i in 0..@cnf[j].size-1
        printf("%d,",@cnf[j][i])
      end
      printf("\n")
    end
    printf("\n## ATM ##\n")
    for j in 0..@ucs
      printf("%d,",@atm[j])
    end
    printf("\n")
  end
end

class NextCnf
  def initialize(c)
    @cnf = c
    @vars = Hash.new(nil)
    @cls = Array.new
    @cnt = 0
  end

  def p_atom
    @cnt += 1
    printf("rule:%2d , atom:%5d\n",@cnt,@vars.size)
  end

  def abs(v)
    if v > 0
      return v
    else return -v
    end
  end

  #### get id ####
  def get_id(v)
    key = abs(v)
    if key == 0
      return
    end
    if @vars[key] == nil
      @vars[key] = @vars.size + 1
      #printf("%d:%s\n",@vars[v],v)
    end
    return @vars[v]
  end

  #### show ####
  def show
    @cls.clear

    # to small
    for i in 0..@cnf.get_size-1
      if @cnf.get_cnfi(i).size == 1
        p "unit!"
      else
        line = @cnf.get_cnfi(i)
        @cls.push(line)
        line.each do |val|
          get_id(val)
        end
      end
    end

    file = sprintf("%s_u.cnf",@cnf.get_body)
    f = open(file,"w")

    #### cnf ####
    f.printf("p cnf %d %d\n",@vars.size,@cls.size)
    @cls.each do |cl|
      f.printf("%s 0\n",cl.join(" "))
    end
    f.printf("%%\n")
=begin

    #### cnf ####
    @vars_ary = Array.new
    @vars.to_a.sort{|a,b| a[1] <=>  b[1]}.each do |var|
      @vars_ary.push(var[0])
    end

    @cls.each do |cl|
      cl.each do |lt|
        if lt < 0
          sign = -1
          id = -lt - 1
        else
          sign = 1
          id = lt - 1
        end

        var = @vars_ary[id]
        if sign == -1
          #f.printf("-")
        end
        #f.printf("%-35s ",var)
      end
      #f.printf("\n")
    end
=end
    f.close
  end
end

################################################################################
# main
################################################################################
c = PreCnf.new(@body)
s = NextCnf.new(c)
s.show
