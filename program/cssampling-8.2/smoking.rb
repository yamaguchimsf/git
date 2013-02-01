#!/usr/bin/ruby

################################################################################
# default
################################################################################
@body = "smoking"
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
class MyConst
  def initialize(body)
    @body = body
    @name = Hash.new(nil)
    file = sprintf("%s.db",@body)
    if File::exist?(file) == true
      f = open(file,"r")
      while line = f.gets
        arg = get_args(line)
        for i in 1..arg.size-1
          if @name[arg[i]] == nil
            @name[arg[i]] = @name.size
          end
        end
      end
      f.close
    end

  end

  def to_a
    return @name.keys
  end

  def to_p
    ary = Array.new
    for i in 0..@name.size-1
      for j in 0..@name.size-1
        a = @name.keys[i]
        b = @name.keys[j]
        ary.push([a,b])
      end
    end
    return ary
  end

  def get_args(line)
    line.chop.gsub(/[\(\)]/,",").gsub(/ /,"").split(",")
  end

  def get_body
    return @body
  end
end

class MySmoking
  def initialize(c)
    @const = c
    @vars = Hash.new(nil)
    @cls = Array.new
  end

  #### rule 1 : S(x) => C(x) ####
  def add_rule1(x)
    var1 = sprintf("S(%s)",x)
    var2 = sprintf("C(%s)",x)

    id1 = get_id(var1)
    id2 = get_id(var2)

    cl = [-id1,id2]

    @cls.push(cl)
  end

  #### rule 2 : F(x,y) => (S(x) <=> S(y))
  def add_rule2(pair)
    x = pair[0]
    y = pair[1]

    var1 = sprintf("F(%s,%s)",x,y)
    var2 = sprintf("S(%s)",x)
    var3 = sprintf("S(%s)",y)

    id1 = get_id(var1)
    id2 = get_id(var2)
    id3 = get_id(var3)

    cl1 = [-id1, -id2, id3]
    cl2 = [-id1, -id3, id2]

    @cls.push(cl1)
    @cls.push(cl2)
  end

  #### get id ####
  def get_id(v)
    if @vars[v] == nil
      @vars[v] = @vars.size + 1
    end

    return @vars[v]
  end

  #### show ####
  def show
    @cls.clear

    # rule1
    @const.to_a.each do |name|
      add_rule1(name)
    end

    # rule2
    @const.to_p.each do |pair|
      add_rule2(pair)
    end

    # show
    file = sprintf("%s.cnf",@const.get_body)
    f = open(file,"w")

    #### cnf ####
    f.printf("p cnf %d %d\n",@vars.size,@cls.size)
    @cls.each do |cl|
      f.printf("%s 0\n",cl.join(" "))
    end
    f.printf("%%\n")

    #### cnf ####
    @vars_ary = Array.new
    @vars.to_a.sort{|a,b| a[1] <=> b[1]}.each do |var|
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
          f.printf("-")
        end
        f.printf("%-25s ",var)
      end
      f.printf("\n")
    end

    f.close
  end
end

################################################################################
# main
################################################################################
c = MyConst.new(@body)
s = MySmoking.new(c)

s.show
