#
# getcomsize.pl
#
# This is a perl script to purse the include file, 
# $JLCSIMROOT/util/inc/trbuff.inc, get the size of common variable,
# and prepare the file for JSFReadJIMCommon.h
#
# (History)
#  1-Feb-2000 A.Miyamoto  Original version.
#  7-July-2000 A.Miyamoto  Switch SIMDST version 203 and 204 automatically
#
#$Id$
#

if ( $ENV{"JLCSIMROOT"} =~ /^$/ ) {
    print "%%Fatal Error in getcomsize.pl\n";
    print "  The environment parameter JLCSIMROOT is undefined.\n";
    exit;
}

$jsfconf=join("/",$ENV{"JSFROOT"},"conf/jsf.version");

$simdstversion=`grep JSF_SIMDST_VERSION $jsfconf`;
@simdstvers=split(" ",$simdstversion);

$trbuffvers=$simdstvers[2];

if ( $trbuffvers == 203 ) {
  $infile=join("/",$ENV{"JLCSIMROOT"},"util","inc","trbuff.inc");
}
elsif( $trbuffvers == 204 ) {
  $infile=join("/",$ENV{"JLCSIMROOT"},"include","jlc","trbuff.inc");
}
else {
  print "Fatal error in getcomsize.pl\n";
  print "Unsupported SIMDST version is specified: $trbuffvers\n";
  exit;
}

print "// SIMDST version for this JSF is $simdstvers[2]\n";

if ( !-e $infile ) { 
  print "Fatal error in getcomsize.pl\n";
  print "$infile does not exist.\n";
  print "May be inconsistent SIMDST version of JSF and JLCSIM.\n";
  print "JSF version is $trbuffvers ( this is specified in the file $jsfconf)\n";
  exit;
}


open(FIN,$infile);

while(<FIN>){
    next if $_ =~ /^\*/ ;
    next if $_ =~ /^C/ ;
    next if $_ !~ /PARAMETER/ ;
    chop;
    s/PARAMETER//g;
    s/\(//g;
    s/\)//g;
    s/ //g;
    @def=split(",");
    foreach $d ( @def ) {
	@form=split("=",$d);
	if ( $form[0] =~ /^MXGEN$/ ) { $mxgen=$form[1]; }
	elsif ( $form[0] =~ /^MXCMB$/ ) { $mxcmb=$form[1]; }
	elsif ( $form[0] =~ /^MXTRK$/ ) { $mxtrk=$form[1]; }
	elsif ( $form[0] =~ /^MXECLS$/ ) { $mxecls=$form[1]; }
	elsif ( $form[0] =~ /^MXHCLS$/ ) { $mxhcls=$form[1]; }
	elsif ( $form[0] =~ /^MXSIH$/ ) { $mxsih=$form[1]; }
	elsif ( $form[0] =~ /^NPECLS$/ ) { $npecls=$form[1]; }
	elsif ( $form[0] =~ /^NPHCLS$/ ) { $nphcls=$form[1]; }
	elsif ( $form[0] =~ /^MXSCLS$/ ) { $mxscls=$form[1]; }
	elsif ( $form[0] =~ /^NPSCLS$/ ) { $npscls=$form[1]; }

    }
}

close(FIN);

if( $npecls != $nphcls ) {
    print " %Fatal Error in getcomsize.pl\n";
    print "  NPECLS($npecls) and NPHCLS($nphcls) is different.\n";
    print "  Please rewrite JSFReadJIMCommon.h\n";
    exit;
}

$nowstring=localtime;

print "//* Common size definition\n";
print "#include \"JSFConfig.h\"\n";
print "const Int_t kJGenMax=$mxgen\;\n";
print "const Int_t kJCmbMax=$mxcmb\;\n";
print "const Int_t kJTrkMax=$mxtrk\;\n";
print "const Int_t kJEClsMax=$mxecls\;\n";
print "const Int_t kJHClsMax=$mxhcls\;\n";
print "const Int_t kJClsSize=$npecls\;\n";
print "const Int_t kJVTXBufSize=$mxsih\;\n";

if ( $trbuffvers >= 204 ) {
  print "const Int_t kJSCLSMax=$mxscls\;\n";
  print "const Int_t kJSCLSSize=$npscls\;\n";
}
print "//* Created from $infile on $nowstring\n";

exit;




