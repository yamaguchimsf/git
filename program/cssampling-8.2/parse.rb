#!/usr/bin/ruby

################################################################################
# default
################################################################################
@body = "language"
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
    @count = 0
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

  def to_p(c,p)
    ary = Array.new
    for i in 0..@name.size-1
      if @name.keys[i].include?(c)
        for j in 0..@name.size-1
          if @name.keys[j].include?(p)
            x = @name.keys[i]
            y = @name.keys[j]
            ary.push([x,y])
          end
        end
      end
    end
    return ary
  end

  def to_pe(c,p) # p is for EXIST
    ary = Array.new
    y = Array.new
    for j in 0..@name.size-1
      if @name.keys[j].include?(p)
        y.push(@name.keys[j])
      end
    end
    for i in 0..@name.size-1
      if @name.keys[i].include?(c)
        x = @name.keys[i]
        z = append(x,y)
        ary.push(z)
      end
    end
    return ary
  end

  def to_t(c,p,r)
    ary = Array.new
    for i in 0..@name.size-1
      if @name.keys[i].include?(c)
        for j in 0..@name.size-1
          if @name.keys[j].include?(p)
            for k in 0..@name.size-1
              if @name.keys[k].include?(r)
                x = @name.keys[i]
                y = @name.keys[j]
                z = @name.keys[k]
                ary.push([x,y,z])
              end
            end
          end
        end
      end
    end
    return ary
  end

  def to_q(c, p, r, q)
    ary = Array.new
    for i in 0..@name.size-1
      if @name.keys[i].include?(c)
        for j in 0..@name.size-1
          if @name.keys[j].include?(p)
            for k in 0..@name.size-1
              if @name.keys[k].include?(r)
                for l in 0..@name.size-1
                  if @name.keys[l].include?(q)
                    x = @name.keys[i]
                    y = @name.keys[j]
                    z = @name.keys[k]
                    w = @name.keys[l]
                    ary.push([x,y,z,w])
                  end
                end
              end
            end
          end
        end
      end
    end
    return ary
  end

  def to_qe(c, p, r, q) # q is for EXIST
    ary = Array.new
    w = Array.new
    for l in 0..@name.size-1
      if @name.keys[l].include?(q)
        w.push(@name.keys[l])
      end
    end
    for i in 0..@name.size-1
      if @name.keys[i].include?(c)
        for j in 0..@name.size-1
          if @name.keys[j].include?(p)
            for k in 0..@name.size-1
              if @name.keys[k].include?(r)
                x = @name.keys[i]
                y = @name.keys[j]
                z = @name.keys[k]
                n = app_t(x,y,z)
                o = app_p(n,w)
                ary.push(o)
              end
            end
          end
        end
      end
    end
    return ary
  end

  def get_args(line)
    return line.chop.gsub(/[\(\)]/,",").gsub(/ /,"").split(",")
  end

  def get_body
    return @body
  end

  def append(a, b)
    c = Array.new
    c.push(a)
    for i in 0..b.size-1
      c.push(b[i])
    end
    return c
  end

  def app_p(a, b)
    c = Array.new
    for i in 0..a.size-1
      c.push(a[i])
    end
    for i in 0..b.size-1
      c.push(b[i])
    end
    return c
  end

  def app_t(a, b, c)
    d = Array.new
    d.push(a)
    d.push(b)
    d.push(c)
    return d
  end

  def p_atom
    printf("rule:%2d , atom:%d\n",@count,@name.size)
    @count += 1
  end
end

class MySmoking
  def initialize(c)
    @const = c
    @vars = Hash.new(nil)
    @cls = Array.new
    @cnt = 0
  end

  def p_atom
    @cnt += 1
    printf("rule:%2d , atom:%5d\n",@cnt,@vars.size)
  end

  #### rule 1 : !tb(c,p,q) v !cl(c,L) v pr(p) ####
  def add_rule1(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("tb(%s,%s,%s)",c,p,q)
    var2 = sprintf("cl(%s,Level_500)",c)
    var3 = sprintf("pr(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1,-id2,id3]
    @cls.push(cl)
  end

  #### rule 2 : !tb(c,p,q) v !st(p) v !cl(c,L)
  def add_rule2(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("tb(%s,%s,%s)",c,p,q)
    var2 = sprintf("st(%s)",p)
    var3 = sprintf("cl(%s,Level_500)",c)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, -id2, -id3]
    @cls.push(cl)
  end

  #### rule 3 : !tb(c,p,q) v !st(p) v !ip(p,P)
  def add_rule3(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("tb(%s,%s,%s)",c,p,q)
    var2 = sprintf("st(%s)",p)
    var3 = sprintf("ip(%s,Pre_Quals)",p)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, -id2, -id3]
    @cls.push(cl)
  end

  #### rule 4 : !tb(c,p,q) v !st(p) v !yp(p,Y)
  def add_rule4(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("tb(%s,%s,%s)",c,p,q)
    var2 = sprintf("st(%s)",p)
    var3 = sprintf("yp(%s,Year_1)",p)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, -id2, -id3]
    @cls.push(cl)
  end

  #### rule 5 : !tab(p,s) v pr(p)
  def add_rule5(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("tab(%s,%s)",p,s)
      var2 = sprintf("pr(%s)",p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 6 : !tab(p,s) v st(s)
  def add_rule6(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("tab(%s,%s)",p,s)
      var2 = sprintf("st(%s)",s)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 7 : !tab(p,s) v hp(p,f)
  def add_rule7(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("tab(%s,%s)",p,s)
      var2 = sprintf("hp(%s,Faculty)",p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 8 : !tab(p,s) v ip(s,P)
  def add_rule8(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("tab(%s,%s)",p,s)
      var2 = sprintf("ip(%s,Pre_Quals)",s)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 9 : !tb(c,p,q) v !cl(c,L) v !ta(c,s,q) v ab(s,p) v tab(s,p)
  def add_rule9(quartett)
    c = quartett[0]; p = quartett[1]; s = quartett[2]; q = quartett[3]

    if p != s
      var1 = sprintf("tb(%s,%s,%s)",c,p,q)
      var2 = sprintf("cl(%s,Level_500)",c)
      var3 = sprintf("ta(%s,%s,%s)",c,s,q)
      var4 = sprintf("ab(%s,%s)",s,p)
      var5 = sprintf("tab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, -id2, -id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 10 : !ab(p,s) v st(s)
  def add_rule10(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("ab(%s,%s)",p,s)
      var2 = sprintf("st(%s)",s)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 11 : !ab(p,s) v pr(s)
  def add_rule11(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("ab(%s,%s)",p,s)
      var2 = sprintf("pr(%s)",s)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 12 : !ab(p,s) v !yp(s,Y)
  def add_rule12(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("ab(%s,%s)",p,s)
      var2 = sprintf("yp(%s,Year_1)",p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 13 : !pu(p,x) v !pu(p,y) v !st(x) v st(y) v pr(y)
  def add_rule13(torio)
    p = torio[0]; x = torio[1]; y = torio[2]

    if x != y
      var1 = sprintf("pu(%s,%s)",p,x)
      var2 = sprintf("pu(%s,%s)",p,y)
      var3 = sprintf("st(%s)",x)
      var4 = sprintf("st(%s)",y)
      var5 = sprintf("pr(%s)",y)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, -id2, -id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 14 : !pu(p,x) v !pu(p,y) v !st(x) v st(y) v ab(x,y) v tab(x,y)
  def add_rule14(torio)
    p = torio[0]; x = torio[1]; y = torio[2]

    if x != y
      var1 = sprintf("pu(%s,%s)",p,x)
      var2 = sprintf("pu(%s,%s)",p,y)
      var3 = sprintf("st(%s)",x)
      var4 = sprintf("st(%s)",y)
      var5 = sprintf("ab(%s,%s)",x,y)
      var6 = sprintf("tab(%s,%s)",x,y)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5); id6 = get_id(var6)
      cl = [-id1, -id2, -id3, id4, id5, id6]
      @cls.push(cl)
    end
  end

  #### rule 15 : !pr(x) v !st(y)
  def add_rule15(pair)
    x = pair[0]; y = pair[1]

    if x != y
      var1 = sprintf("pr(%s)",x)
      var2 = sprintf("st(%s)",y)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 16 : !st(x) v ab(x,y) v tab(x,y)
  def add_rule16(pair)
    x = pair[0]; y = pair[1]

    if x != y
      var1 = sprintf("st(%s)",x)
      var2 = sprintf("ab(%s,%s)",x,y)
      var3 = sprintf("tab(%s,%s)",x,y)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      cl = [-id1, -id2, id3]
      @cls.push(cl)
    end
  end

  #### rule 17 : !pr(p) v !hp(p,F) v tb(c,p,q)
  def add_rule17(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("pr(%s)",p)
    var2 = sprintf("hp(%s,Faculty)",p)
    var3 = sprintf("tb(%s,%s,%s)",c,p,q)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, -id2, id3]
    @cls.push(cl)
  end

  #### rule 18 : !ip(s,P) v !yp(s,Y)
  def add_rule18(name)
    s = name[0]

    var1 = sprintf("ip(%s,Post_Quals)",s)
    var2 = sprintf("yp(%s,Year_1)",s)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, -id2]
    @cls.push(cl)
  end

  #### rule 19 : !ip(s,P) v !ip(s,P)
  def add_rule19(name)
    s = name[0]

    var1 = sprintf("ip(%s,Post_Quals)",s)
    var2 = sprintf("ip(%s,Post_Generals)",s)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, -id2]
    @cls.push(cl)
  end

  #### rule 20 : !ip(s,P) v !ip(s,P)
  def add_rule20(name)
    s = name[0]

    var1 = sprintf("ip(%s,Post_Generals)",s)
    var2 = sprintf("ip(%s,Pre_Quals)",s)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, -id2]
    @cls.push(cl)
  end

  #### rule 21 : !pr(p) v hp(p,f) v hp(p,f) v hp(p,f) v hp(p,f) v hp(p,f)
  def add_rule21(name)
    s = name[0]

    var1 = sprintf("pr(%s)",s)
    var2 = sprintf("hp(%s,Faculty)",s)
    var3 = sprintf("hp(%s,Faculty_affiliate)",s)
    var4 = sprintf("hp(%s,Faculty_adjunct)",s)
    var5 = sprintf("hp(%s,Faculty_emeritus)",s)
    var6 = sprintf("hp(%s,Faculty_visiting)",s)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    id4 = get_id(var4); id5 = get_id(var5); id6 = get_id(var6)
    cl = [-id1, id2, id3, id4, id5, id6]
    @cls.push(cl)
  end

  #### rule 22 : !hp(p,f) v !ab(s,p)
  def add_rule22(pair)
    p = pair[0]; s = pair[1]

    if p != s
      var1 = sprintf("hp(%s,Faculty_visiting)",p)
      var2 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 23 : !pr(x) v !hp(x,f) v ab(s,x) v tab(s,x)
  def add_rule23(pair)
    x = pair[0]; s = pair[1]

    if x != s
      var1 = sprintf("pr(%s)",x)
      var2 = sprintf("hp(%s,Faculty)",x)
      var3 = sprintf("ab(%s,%s)",s,x)
      var4 = sprintf("tab(%s,%s)",s,x)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3); id4 = get_id(var4)
      cl = [-id1, -id2, id3, id4]
      @cls.push(cl)
    end
  end

  #### rule 24 : !st(p) v yp(p,Y) v ta(c,p,q)
  def add_rule24(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("st(%s)",p)
    var2 = sprintf("yp(%s,Year_1)",p)
    var3 = sprintf("ta(%s,%s,%s)",c,p,q)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, id2, id3]
    @cls.push(cl)
  end

  #### rule 25 : !tab(x,y) v !hp(x,f)
  def add_rule25(pair)
    x = pair[0]; y = pair[1]

    if x != y
      var1 = sprintf("tab(%s,%s)",x,y)
      var2 = sprintf("hp(%s,Faculty_visiting)",x)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 26 : !tab(x,y) v yp(x,Y) v yp(x,Y)
  def add_rule26(pair)
    x = pair[0]; y = pair[1]

    if x != y
      var1 = sprintf("tab(%s,%s)",x,y)
      var2 = sprintf("yp(%s,Year_1)",x)
      var3 = sprintf("yp(%s,Year_2)",x)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      cl = [-id1, id2, id3]
      @cls.push(cl)
    end
  end

  #### rule 27 : !tab(x,y) v ip(x,P)
  def add_rule27(pair)
    x = pair[0]; y = pair[1]

    if x != y
      var1 = sprintf("tab(%s,%s)",x,y)
      var2 = sprintf("ip(%s,Pre_Quals)",x)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 28 : Exist  !st(x) v ab(x,y) v tab(x,y)
  def add_rule28(pair)
    x = pair[0]; y = pair
    cl = Array.new

    var1 = sprintf("st(%s)",x)
    id1 = get_id(var1)
    cl.push(-id1)
    for j in 1..y.size-1
      if x != y[j]
        var2 = sprintf("ab(%s,%s)",x,y[j])
        var3 = sprintf("tab(%s,%s)",x,y[j])
        id2 = get_id(var2); id3 = get_id(var3)
        cl.push(id2); cl.push(id3)
      end
    end
    @cls.push(cl)
  end

  #### rule 29 : Exist  !pr(x) v hp(x,F) v ab(y,x)
  def add_rule29(pair)
    x = pair[0]; y = pair
    cl = Array.new

    var1 = sprintf("pr(%s)",x); var2 = sprintf("hp(%s,Faculty_visiting)",x)
    id1 = get_id(var1); id2 = get_id(var2)
    cl.push(-id1); cl.push(id2)
    for j in 1..y.size-1
      if x != y[j]
        var3 = sprintf("ab(%s,%s)",y[j],x)
        id3 = get_id(var3)
        cl.push(id3)
      end
    end
    @cls.push(cl)
  end

  #### rule 30 : Exist  !tb(c,x,q) v ta(c,y,q)
  def add_rule30(quartett)
    c = quartett[0]; q = quartett[1]; x = quartett[2]; y = quartett
    cl = Array.new

    var1 = sprintf("tb(%s,%s,%s)",c,x,q)
    id1 = get_id(var1)
    cl.push(-id1)
    for j in 3..y.size-1
      var2 = sprintf("ta(%s,%s,%s)",c,y[j],q)
      id2 = get_id(var2);
      cl.push(id2)
    end
    @cls.push(cl)
  end

  #### rule 31 : Exist  !ta(c,x,q) v tb(c,y,q)
  def add_rule31(quartett)
    c = quartett[0]; q = quartett[1]; x = quartett[2]; y = quartett
    cl = Array.new

    var1 = sprintf("ta(%s,%s,%s)",c,x,q)
    id1 = get_id(var1)
    cl.push(-id1)
    for j in 3..y.size-1
      var2 = sprintf("tb(%s,%s,%s)",c,y[j],q)
      id2 = get_id(var2);
      cl.push(id2)
    end
    @cls.push(cl)
  end

  #### rule 32 : !st(x) v ip(x,P) v ip(x,P) v ip(x,P)
  def add_rule32(name)
    x = name[0]

    var1 = sprintf("st(%s)",x)
    var2 = sprintf("ip(%s,Pre_Quals)",x)
    var3 = sprintf("ip(%s,Post_Quals)",x)
    var4 = sprintf("ip(%s,Post_Generals)",x)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3); id4 = get_id(var4)
    cl = [-id1, id2, id3, id4]
    @cls.push(cl)
  end

  #### rule 33 : !ip(x,P) v !ip(x,P)
  def add_rule33(name)
    x = name[0]

    var1 = sprintf("ip(%s,Pre_Quals)",x)
    var2 = sprintf("ip(%s,Post_Quals)",x)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, -id2]
    @cls.push(cl)
  end

  #### rule 34 : !hp(x,y) v !hp(x,z) v spo(y,z)
  def add_rule34(torio)
    x = torio[0]; y = torio[1]; z = torio[2]

    var1 = sprintf("hp(%s,%s)",x,y)
    var2 = sprintf("hp(%s,%s)",x,z)
    var3 = sprintf("spo(%s,%s)",y,z)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, -id2, id3]
    @cls.push(cl)
  end

  #### rule 35 : Exist !ip(x,P) v pu(p,x)
  def add_rule35(pair)
    x = pair[0]; p = pair
    cl = Array.new

    var1 = sprintf("ip(%s,Post_Generals)",x)
    id1 = get_id(var1)
    cl.push(-id1)
    for j in 1..p.size-1
      var2 = sprintf("pu(%s,%s)",p[j],x)
      id2 = get_id(var2)
      cl.push(id2)
    end
    @cls.push(cl)
  end

  #### rule 36 : Exist !pr(x) v hp(x,y)
  def add_rule36(pair)
    x = pair[0]; y = pair
    cl = Array.new

    var1 = sprintf("pr(%s)",x) 
    id1 = get_id(var1)
    cl.push(-id1)
    for j in 1..y.size-1
      var2 = sprintf("hp(%s,%s)",x,y[j])
      id2 = get_id(var2)
      cl.push(id2)
    end
    @cls.push(cl)
  end

  #### rule 37 : ab(a,a)
  def add_rule37(name)
    a = name[0]

    var1 = sprintf("ab(%s,%s)",a,a)
    id1 = get_id(var1)
    cl = [-id1]
    @cls.push(cl)
  end

  #### rule 38 : tab(a,a)
  def add_rule38(name)
    x = name[0]

    var1 = sprintf("tab(%s,%s)",x,x)
    id1 = get_id(var1)
    cl = [-id1]
    @cls.push(cl)
  end

  #### rule 39 : !ab(a,b) v !ab(b,a)
  def add_rule39(pair)
    a = pair[0]; b = pair[1]

    if a != b
      var1 = sprintf("ab(%s,%s)",a,b)
      var2 = sprintf("ab(%s,%s)",b,a)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 40 : !tab(a,b) v !tab(b,a)
  def add_rule40(pair)
    a = pair[0]; b = pair[1]

    if a != b
      var1 = sprintf("tab(%s,%s)",a,b)
      var2 = sprintf("tab(%s,%s)",b,a)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 41 : !ab(s,p) v spe(p,q) v !ab(s,q)
  def add_rule41(torio)
    s = torio[0]; p = torio[1]; q = torio[2]

    if s != p
      if s != q
        var1 = sprintf("ab(%s,%s)",s,p)
        var2 = sprintf("spe(%s,%s)",p,q)
        var3 = sprintf("ab(%s,%s)",s,q)
        id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
        cl = [-id1, id2, -id3]
        @cls.push(cl)
      end
    end
  end

  #### rule 42 : !tab(s,p) v spe(p,q) v !tab(s,q)
  def add_rule42(torio)
    s = torio[0]; p = torio[1]; q = torio[2]

    if s != p
      if s != q
        var1 = sprintf("tab(%s,%s)",s,p)
        var2 = sprintf("spe(%s,%s)",p,q)
        var3 = sprintf("tab(%s,%s)",s,q)
        id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
        cl = [-id1, id2, -id3]
        @cls.push(cl)
      end
    end
  end

  #### rule 43 : !tab(s,p) v !ab(s,q)
  def add_rule43(torio)
    s = torio[0]; p = torio[1]; q = torio[2]

    if s != p
      if s != q
        var1 = sprintf("tab(%s,%s)",s,p)
        var2 = sprintf("ab(%s,%s)",s,q)
        id1 = get_id(var1); id2 = get_id(var2)
        cl = [-id1, -id2]
        @cls.push(cl)
      end
    end
  end

  #### rule 44 : !ip(s,P) v !ab(s,p)
  def add_rule44(pair)
    s = pair[0]; p = pair[1]

    if s != p
      var1 = sprintf("ip(%s,Pre_Quals)",s)
      var2 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 45 : !ip(s,P) v !tab(s,p)
  def add_rule45(pair)
    s = pair[0]; p = pair[1]

    if s != p
      var1 = sprintf("ip(%s,Post_Quals)",s)
      var2 = sprintf("tab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 46 : !ip(s,P) v !tab(s,p)
  def add_rule46(pair)
    s = pair[0]; p = pair[1]

    if s != p
      var1 = sprintf("ip(%s,Post_Generals)",s)
      var2 = sprintf("tab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, -id2]
      @cls.push(cl)
    end
  end

  #### rule 47 : !ip(s,P) v !tb(c,p,q) v !ta(c,s,q) v cl(c,L) v ab(s,p)
  def add_rule47(quartett)
    s = quartett[0]; p = quartett[1]; c = quartett[2]; q = quartett[3]

    if s != p
      var1 = sprintf("ip(%s,Post_Quals)",s)
      var2 = sprintf("tb(%s,%s,%s)",c,p,q)
      var3 = sprintf("ta(%s,%s,%s)",c,s,q)
      var4 = sprintf("cl(%s,Level_100)",c)
      var5 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, -id2, -id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 48 : !ip(s,P) v !tb(c,p,q) v ta(c,s,q) v cl(c,L) v ab(s,p)
  def add_rule48(quartett)
    s = quartett[0]; p = quartett[1]; c = quartett[2]; q = quartett[3]

    if s != p
      var1 = sprintf("ip(%s,Post_Quals)",s)
      var2 = sprintf("tb(%s,%s,%s)",c,p,q)
      var3 = sprintf("ta(%s,%s,%s)",c,s,q)
      var4 = sprintf("cl(%s,Level_100)",c)
      var5 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, -id2, id3, id4, id5]
      @cls.push(cl)
    end
end

  #### rule 49 : !ip(s,P) v tb(c,p,q) v !ta(c,s,q) v cl(c,L) v ab(s,p)
  def add_rule49(quartett)
    s = quartett[0]; p = quartett[1]; c = quartett[2]; q = quartett[3]

    if s != p
      var1 = sprintf("ip(%s,Post_Quals)",s)
      var2 = sprintf("tb(%s,%s,%s)",c,p,q)
      var3 = sprintf("ta(%s,%s,%s)",c,s,q)
      var4 = sprintf("cl(%s,Level_100)",c)
      var5 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, id2, -id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 50 : !ip(s,P) v !tb(c,p,q) v !ta(c,s,q) v cl(c,L) v ab(s,p)
  def add_rule50(quartett)
    s = quartett[0]; p = quartett[1]; c = quartett[2]; q = quartett[3]

    if s != p
      var1 = sprintf("ip(%s,Post_Generals)",s)
      var2 = sprintf("tb(%s,%s,%s)",c,p,q)
      var3 = sprintf("ta(%s,%s,%s)",c,s,q)
      var4 = sprintf("cl(%s,Level_100)",c)
      var5 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, -id2, -id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 51 : !ip(s,P) v !tb(c,p,q) v ta(c,s,q) v cl(c,L) v ab(s,p)
  def add_rule51(quartett)
    s = quartett[0]; p = quartett[1]; c = quartett[2]; q = quartett[3]

    if s != p
      var1 = sprintf("ip(%s,Post_Generals)",s)
      var2 = sprintf("tb(%s,%s,%s)",c,p,q)
      var3 = sprintf("ta(%s,%s,%s)",c,s,q)
      var4 = sprintf("cl(%s,Level_100)",c)
      var5 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, -id2, id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 52 : !ip(s,P) v tb(c,p,q) v !ta(c,s,q) v cl(c,L) v ab(s,p)
  def add_rule52(quartett)
    s = quartett[0]; p = quartett[1]; c = quartett[2]; q = quartett[3]

    if s != p
      var1 = sprintf("ip(%s,Post_Generals)",s)
      var2 = sprintf("tb(%s,%s,%s)",c,p,q)
      var3 = sprintf("ta(%s,%s,%s)",c,s,q)
      var4 = sprintf("cl(%s,Level_100)",c)
      var5 = sprintf("ab(%s,%s)",s,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, id2, -id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 53 : !pu(t,a) v !pu(t,b) v spe(a,b) v ab(a,b) v ab(b,a)
  def add_rule53(torio)
    t = torio[0]; a = torio[1]; b = torio[2]

    if a != b
      var1 = sprintf("pu(%s,%s)",t,a)
      var2 = sprintf("pu(%s,%s)",t,b)
      var3 = sprintf("spe(%s,%s)",a,b)
      var4 = sprintf("ab(%s,%s)",a,b)
      var5 = sprintf("ab(%s,%s)",b,a)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5)
      cl = [-id1, -id2, id3, id4, id5]
      @cls.push(cl)
    end
  end

  #### rule 54 : !pu(t,a) v !pu(t,b) v spe(a,b) v !pr(a) v !st(b) v ab(b,a)
  def add_rule54(torio)
    t = torio[0]; a = torio[1]; b = torio[2]

    if a != b
      var1 = sprintf("pu(%s,%s)",t,a)
      var2 = sprintf("pu(%s,%s)",t,b)
      var3 = sprintf("spe(%s,%s)",a,b)
      var4 = sprintf("pr(%s)",a)
      var5 = sprintf("st(%s)",b)
      var6 = sprintf("ab(%s,%s)",b,a)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      id4 = get_id(var4); id5 = get_id(var5); id6 = get_id(var6)
      cl = [-id1, -id2, id3, -id4, -id5, id6]
      @cls.push(cl)
    end
  end

  #### rule 55 : !ab(s,p) v !pu(t,s) v pu(t,p)
  def add_rule55(torio)
    s = torio[0]; p = torio[1]; t = torio[2]

    if s != p
      var1 = sprintf("ab(%s,%s)",s,p)
      var2 = sprintf("pu(%s,%s)",t,s)
      var3 = sprintf("pu(%s,%s)",t,p)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      cl = [-id1, -id2, id3]
      @cls.push(cl)
    end
  end

  #### rule 56 : !tb(c,p,q) v pr(p)
  def add_rule56(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("tb(%s,%s,%s)",c,p,q)
    var2 = sprintf("pr(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, id2]
    @cls.push(cl)
  end

  #### rule 57 : !hp(p,x) v pr(p)
  def add_rule57(pair)
    p = pair[0]; x = pair[1]

    var1 = sprintf("hp(%s,%s)",p,x)
    var2 = sprintf("pr(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, id2]
    @cls.push(cl)
  end

  #### rule 58 : !ab(s,p) v st(s)
  def add_rule58(pair)
    s = pair[0]; p = pair[1]

    if s != p
      var1 = sprintf("ab(%s,%s)",s,p)
      var2 = sprintf("st(%s)",s)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 59 : !ab(s,p) v pr(p)
  def add_rule59(pair)
    s = pair[0]; p = pair[1]

    if s != p
      var1 = sprintf("ab(%s,%s)",s,p)
      var2 = sprintf("pr(%s)",p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 60 : !ip(p,x) v st(p)
  def add_rule60(pair)
    p = pair[0]; x = pair[1]

    var1 = sprintf("ip(%s,%s)",p,x)
    var2 = sprintf("st(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, id2]
    @cls.push(cl)
  end

  #### rule 61 : !tab(s,p) v st(s)
  def add_rule61(pair)
    s = pair[0]; p = pair[1]

    if s != p
      var1 = sprintf("tab(%s,%s)",s,p)
      var2 = sprintf("st(%s)",s)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 62 : !tab(s,p) v pr(p)
  def add_rule62(pair)
    s = pair[0]; p = pair[1]

    if s != p
      var1 = sprintf("tab(%s,%s)",s,p)
      var2 = sprintf("pr(%s)",p)
      id1 = get_id(var1); id2 = get_id(var2)
      cl = [-id1, id2]
      @cls.push(cl)
    end
  end

  #### rule 63 : !yp(p,x) v st(p)
  def add_rule63(pair)
    p = pair[0]; x = pair[1]

    var1 = sprintf("yp(%s,%s)",p,x)
    var2 = sprintf("st(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, id2]
    @cls.push(cl)
  end

  #### rule 64 : !ta(c,p,q) v st(p)
  def add_rule64(torio)
    c = torio[0]; p = torio[1]; q = torio[2]

    var1 = sprintf("ta(%s,%s,%s)",c,p,q)
    var2 = sprintf("st(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, id2]
    @cls.push(cl)
  end

  #### rule 65 : st(p) v pr(p)
  def add_rule65(name)
    p = name[0]

    var1 = sprintf("st(%s)",p)
    var2 = sprintf("pr(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [id1, id2]
    @cls.push(cl)
  end

  #### rule 66 : !st(p) v !pr(p)
  def add_rule66(name)
    p = name[0]

    var1 = sprintf("st(%s)",p)
    var2 = sprintf("pr(%s)",p)
    id1 = get_id(var1); id2 = get_id(var2)
    cl = [-id1, -id2]
    @cls.push(cl)
  end

  #### rule 67 : !ip(p,x) v sph(x,y) v !ip(p,y)
  def add_rule67(torio)
    p = torio[0]; x = torio[1]; y = torio[2]

    var1 = sprintf("ip(%s,%s)",p,x)
    var2 = sprintf("sph(%s,%s)",x,y)
    var3 = sprintf("ip(%s,%s)",p,y)
    id1 = get_id(var1); id2 = get_id(var2);  id3 = get_id(var3)
    cl = [-id1, id2, -id3]
    @cls.push(cl)
  end

  #### rule 68 : !yp(p,x) v si(x,y) v !yp(p,y)
  def add_rule68(torio)
    p = torio[0]; x = torio[1]; y = torio[2]

    var1 = sprintf("yp(%s,%s)",p,x)
    var2 = sprintf("si(%s,%s)",x,y)
    var3 = sprintf("yp(%s,%s)",p,y)
    id1 = get_id(var1); id2 = get_id(var2);  id3 = get_id(var3)
    cl = [-id1, id2, -id3]
    @cls.push(cl)
  end

  #### rule 69 : !tb(x,p,q) v sc(x,y) v !tb(y,p,q)
  def add_rule69(quartett)
    x = quartett[0]; y = quartett[1]; p = quartett[2]; q = quartett[3]

    var1 = sprintf("tb(%s,%s,%s)",x,p,q)
    var2 = sprintf("sc(%s,%s)",x,y)
    var3 = sprintf("tb(%s,%s,%s)",y,p,q)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, id2, -id3]
    @cls.push(cl)
  end

  #### rule 70 : !tb(c,x,q) v spe(x,y) v !tb(c,y,q)
  def add_rule70(quartett)
    c = quartett[0]; x = quartett[1]; y = quartett[2]; q = quartett[3]

    if x != y
      var1 = sprintf("tb(%s,%s,%s)",c,x,q)
      var2 = sprintf("spe(%s,%s)",x,y)
      var3 = sprintf("tb(%s,%s,%s)",c,y,q)
      id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
      cl = [-id1, id2, -id3]
      @cls.push(cl)
    end
  end

  #### rule 71 : !ta(x,p,q) v sc(x,y) v !ta(y,p,q)
  def add_rule71(quartett)
    x = quartett[0]; y = quartett[1]; p = quartett[2]; q = quartett[3]

    var1 = sprintf("ta(%s,%s,%s)",x,p,q)
    var2 = sprintf("sc(%s,%s)",x,y)
    var3 = sprintf("ta(%s,%s,%s)",y,p,q)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, id2, -id3]
    @cls.push(cl)
  end

  #### rule 72 : !ta(c,x,q) v spe(x,y) v !ta(c,y,q)
  def add_rule72(quartett)
    c = quartett[0]; x = quartett[1]; y = quartett[2]; q = quartett[3]

    var1 = sprintf("ta(%s,%s,%s)",c,x,q)
    var2 = sprintf("spe(%s,%s)",x,y)
    var3 = sprintf("ta(%s,%s,%s)",c,y,q)
    id1 = get_id(var1); id2 = get_id(var2); id3 = get_id(var3)
    cl = [-id1, id2, -id3]
    @cls.push(cl)
  end

  #### get id ####
  def get_id(v)
    if @vars[v] == nil
      @vars[v] = @vars.size + 1
      #printf("%d:%s\n",@vars[v],v)
    end
    return @vars[v]
  end

  #### show ####
  def show
    @cls.clear

    v1 = sprintf("Course");  # course
    v2 = sprintf("Person");  # person
    v3 = sprintf("_0");      # quarter
    v4 = sprintf("Faculty"); # position
    v5 = sprintf("Title");   # title
    v6 = sprintf("Project"); # project
    v7 = sprintf("Level");   # level
    v8 = sprintf("ls");      # phase
    v9 = sprintf("Year");    # integer

    @const.to_t(v1,v2,v3).each do |torio|    # rule1
      add_rule1(torio)
    end
    p_atom
    @const.to_t(v1,v2,v3).each do |torio|    # rule2
      add_rule2(torio)
    end
    p_atom
    @const.to_t(v1,v2,v3).each do |torio|    # rule3
      add_rule3(torio)
    end
    p_atom
    @const.to_t(v1,v2,v3).each do |torio|    # rule4
      add_rule4(torio)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule5
      add_rule5(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule6
      add_rule6(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule7
      add_rule7(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule8
      add_rule8(pair)
    end
    p_atom
    @const.to_q(v1,v2,v2,v3).each do |quartett| # rule9
      add_rule9(quartett)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule10
      add_rule10(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule11
      add_rule11(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule12
      add_rule12(pair)
    end
    p_atom
    @const.to_t(v5,v2,v2).each do |torio|    # rule13
      add_rule13(torio)
    end
    p_atom
    @const.to_t(v5,v2,v2).each do |torio|    # rule14
      add_rule14(torio)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule15
      add_rule15(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule16
      add_rule16(pair)
    end
    p_atom
    @const.to_t(v1,v2,v3).each do |torio|    # rule17
      add_rule17(torio)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule18
      add_rule18(name)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule19
      add_rule19(name)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule20
      add_rule20(name)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule21
      add_rule21(name)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule22
       add_rule22(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule23
      add_rule23(pair)
    end
    p_atom
    @const.to_t(v1,v2,v3).each do |torio|    # rule24
      add_rule24(torio)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule25
      add_rule25(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule26
      add_rule26(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule27
      add_rule27(pair)
    end
    p_atom
    @const.to_pe(v2,v2).each do |pair|    # rule28
      add_rule28(pair)
    end
    p_atom
    @const.to_pe(v2,v2).each do |pair|    # rule29
      add_rule29(pair)
    end
    p_atom
    @const.to_qe(v1,v3,v2,v2).each do |quartett|    # rule30
      add_rule30(quartett)
    end
    p_atom
    @const.to_qe(v1,v3,v2,v2).each do |quartett|    # rule31
      add_rule31(quartett)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule32
      add_rule32(name)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule33
      add_rule33(name)
    end
    p_atom
    @const.to_t(v2,v4,v4).each do |torio|    # rule34
      add_rule34(torio)
    end
    p_atom
    @const.to_pe(v2,v5).each do |pair|    # rule35
      add_rule35(pair)
    end
    p_atom
    @const.to_pe(v2,v4).each do |pair|    # rule36
      add_rule36(pair)
    end
    p_atom
=begin
    @const.to_a(v2).each do |name|    # rule37
      add_rule37(name)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule38
      add_rule38(name)
    end
    p_atom
=end
    @const.to_p(v2,v2).each do |pair|    # rule39
      add_rule39(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule40
      add_rule40(pair)
    end
    p_atom
    @const.to_t(v2,v2,v2).each do |torio|    # rule41
      add_rule41(torio)
    end
    p_atom
    @const.to_t(v2,v2,v2).each do |torio|    # rule42
      add_rule42(torio)
    end
    p_atom
    @const.to_t(v2,v2,v2).each do |torio|    # rule43
     add_rule43(torio)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule44
     add_rule44(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule45
      add_rule45(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule46
      add_rule46(pair)
    end
    p_atom
    @const.to_q(v2,v2,v1,v3).each do |quartett|    # rule47
      add_rule47(quartett)
    end
    p_atom
    @const.to_q(v2,v2,v1,v3).each do |quartett|    # rule48
      add_rule48(quartett)
    end
    p_atom
    @const.to_q(v2,v2,v1,v3).each do |quartett|    # rule49
      add_rule49(quartett)
    end
    p_atom
    @const.to_q(v2,v2,v1,v3).each do |quartett|    # rule50
      add_rule50(quartett)
    end
    p_atom
    @const.to_q(v2,v2,v1,v3).each do |quartett|    # rule51
      add_rule51(quartett)
    end
    p_atom
    @const.to_q(v2,v2,v1,v3).each do |quartett|    # rule52
      add_rule52(quartett)
    end
    p_atom
    @const.to_t(v5,v2,v2).each do |torio|    # rule53
      add_rule53(torio)
    end
    p_atom
    @const.to_t(v5,v2,v2).each do |torio|    # rule54
      add_rule54(torio)
    end
    p_atom
    @const.to_t(v2,v2,v5).each do |torio|    # rule55
      add_rule55(torio)
    end
    p_atom
    @const.to_t(v1,v2,v3).each do |torio|    # rule56
      add_rule56(torio)
    end
    p_atom
    @const.to_p(v2,v4).each do |pair|    # rule57
      add_rule57(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule58
      add_rule58(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule59
      add_rule59(pair)
    end
    p_atom
    @const.to_p(v2,v8).each do |pair|    # rule60
      add_rule60(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule61
      add_rule61(pair)
    end
    p_atom
    @const.to_p(v2,v2).each do |pair|    # rule62
      add_rule62(pair)
    end
    p_atom
    @const.to_p(v2,v9).each do |pair|    # rule63
      add_rule63(pair)
    end
    p_atom
    @const.to_t(v1,v2,v3).each do |torio|    # rule64
      add_rule64(torio)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule65
      add_rule65(name)
    end
    p_atom
    @const.to_a(v2).each do |name|    # rule66
      add_rule66(name)
    end
    p_atom
    @const.to_t(v2,v8,v8).each do |torio|    # rule67
      add_rule67(torio)
    end
    p_atom
    @const.to_t(v2,v9,v9).each do |torio|    # rule68
      add_rule68(torio)
    end
    p_atom
    @const.to_q(v1,v1,v2,v3).each do |quartett|    # rule69
      add_rule69(quartett)
    end
    p_atom
    @const.to_q(v1,v2,v2,v3).each do |quartett|    # rule70
      add_rule70(quartett)
    end
    p_atom
    @const.to_q(v1,v1,v2,v3).each do |quartett|    # rule71
      add_rule71(quartett)
    end
    p_atom
    @const.to_q(v1,v2,v2,v3).each do |quartett|    # rule72
      add_rule72(quartett)
    end
    p_atom

    # show
    file = sprintf("%s.cnf",@const.get_body)
    f = open(file,"w")

    #### cnf ####
    f.printf("p cnf %d %d\n",@vars.size,@cls.size)
    @cls.each do |cl|
      f.printf("%s 0\n",cl.join(" "))
    end
    #f.printf("%%\n")

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

    f.close
  end
end

################################################################################
# main
################################################################################
c = MyConst.new(@body)
s = MySmoking.new(c)

s.show

