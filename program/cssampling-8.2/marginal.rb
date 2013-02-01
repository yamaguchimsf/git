#!/usr/bin/ruby

################################################################################
# default
################################################################################
@body1 = "hage"
@body2 = "hige"
@body3 = "huge"
@body4 = "hege"
@body5 = "hoge"
@num   = "1"
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
  opts.on("-a [string]"){ |f|
    @body1 = f
  }
  opts.on("-b [string]"){ |f|
    @body2 = f
  }
  opts.on("-c [string]"){ |f|
    @body3 = f
  }
  opts.on("-d [string]"){ |f|
    @body4 = f
  }
  opts.on("-e [string]"){ |f|
    @body5 = f
  }
  opts.on("-z [string]"){ |f|
    @num = f
  }
  # parse
  opts.parse!(ARGV)
}

################################################################################
# Class
################################################################################
class MyProblem
  def initialize(body1, body2, body3, body4, body5, num)
    @size=4
    @body1 = body1
    @body2 = body2
    @body3 = body3
    @body4 = body4
    @body5 = body5
    @num   = num.to_i
    @cs    = Array.new(@size*@num)
    @ss    = Array.new(@size*@num)
    @ave_cs= Array.new(@size)
    @ave_ss= Array.new(@size)
    @cov_cs= Array.new(@size)
    @cov_ss= Array.new(@size)
    get_file
    #make_file
  end

  def get_file
    idx=0
    file = sprintf("%s",@body1)
    if File::exist?(file) == true
      f = open(file,"r")
      while line = f.gets
        arg=get_args(line)
        @cs[idx]=arg[1].to_f
        @ss[idx]=arg[2].to_f
        idx+=1
      end
      f.close
    elsif
      printf("file1 don't exist!\n")
      exit
    end

    if @num > 1
      #idx=0
      file = sprintf("%s",@body2)
      if File::exist?(file) == true
        f = open(file,"r")
        while line = f.gets
          arg=get_args(line)
          @cs[idx]=arg[1].to_f
          @ss[idx]=arg[2].to_f
          idx+=1
        end
        f.close
      elsif
        printf("file2 don't exist!\n")
        exit
      end
    end

    if @num > 2
      file = sprintf("%s",@body3)
      if File::exist?(file) == true
        f = open(file,"r")
        while line = f.gets
          arg=get_args(line)
          @cs[idx]=arg[1].to_f
          @ss[idx]=arg[2].to_f
          idx+=1
        end
        f.close
      elsif
        printf("file3 don't exist!\n")
        exit
      end
    end

    if @num > 3
      file = sprintf("%s",@body4)
      if File::exist?(file) == true
        f = open(file,"r")
        while line = f.gets
          arg=get_args(line)
          @cs[idx]=arg[1].to_f
          @ss[idx]=arg[2].to_f
          idx+=1
        end
        f.close
      elsif
        printf("file4 don't exist!\n")
        exit
      end
    end

    if @num > 4
      file = sprintf("%s",@body5)
      if File::exist?(file) == true
        f = open(file,"r")
        while line = f.gets
          arg=get_args(line)
          @cs[idx]=arg[1].to_f
          @ss[idx]=arg[2].to_f
          idx+=1
        end
        f.close
      elsif
        printf("file5 don't exist!\n")
        exit
      end
    end
  end

  def make_file
    file = sprintf("%s.dat",@body1)
    f = open(file,"w")
    for i in 0..@data1.size-1
      f.printf("%f %f\n",@data1[i],@data2[i])
    end
    f.close
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
    return line.chop.split(" ")
  end

  def show
    for j in 0..@size-1
      @ave_cs[j] = 0
      @ave_ss[j] = 0
      @cov_cs[j] = 0
      @cov_ss[j] = 0
    end

    # calc average
    for j in 0..@num-1
      for i in 0..@size-1
        @ave_cs[i] += @cs[j*4+i]
        @ave_ss[i] += @ss[j*4+i]
      end
    end

    for i in 0..@size-1
      @ave_cs[i] /= @num
      @ave_ss[i] /= @num
    end

    # calc covariance
    for j in 0..@num-1
      for i in 0..@size-1
        @cov_cs[i] += (@ave_cs[i] - @cs[j*4+i])**2
        @cov_ss[i] += (@ave_ss[i] - @ss[j*4+i])**2
      end
    end
    for i in 0..@size-1
      @cov_cs[i] /= @num
      @cov_ss[i] /= @num
    end
    for i in 0..@size-1
      sq = Math::sqrt(@cov_cs[i])
      @cov_cs[i] = sq
      sq = Math::sqrt(@cov_ss[i])
      @cov_ss[i] = sq
    end

    # print
    for i in 0..@size-1
      printf("%d %f %f %f %f\n",i+1,@ave_cs[i],@ave_ss[i],@cov_cs[i],@cov_ss[i])
    end
  end
end

################################################################################
# main
################################################################################
p = MyProblem.new(@body1, @body2, @body3, @body4, @body5, @num)
p.show
