#
# getcomsize.pl
#
# This is a perl script to purse the include file, 
# $JLCSIMROOT/util/inc/trbuff.inc, get the size of common variable,
# and prepare the file for JSFReadJIMCommon.h
#
# (History)
#  1-Feb-2000 A.Miyamoto  Original version.
#
#$Id$
#

if ( $ENV{"JLCSIMROOT"} =~ /^$/ ) {
    print "%%Fatal Error in getcomsize.pl\n";
    print "  The environment parameter JLCSIMROOT is undefined.\n";
    exit;
}

$infile=join("/",$ENV{"JLCSIMROOT"},"util","inc","trbuff.inc");


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
    }
}

close(FIN);

if( $npecls != $nphcls ) {
    print "%%Fatal Error in getcomsize.pl\n";
    print "  NPECLS($npecls) and NPHCLS($nphcls) is different.\n";
    print "  Please rewrite JSFReadJIMCommon.h\n";
    exit;
}

$nowstring=localtime;
print "//* Common size definition\n";
print "const Int_t kJGenMax=$mxgen\;\n";
print "const Int_t kJCmbMax=$mxcmb\;\n";
print "const Int_t kJTrkMax=$mxtrk\;\n";
print "const Int_t kJEClsMax=$mxecls\;\n";
print "const Int_t kJHClsMax=$mxhcls\;\n";
print "const Int_t kJClsSize=$npecls\;\n";
print "const Int_t kJVTXBufSize=$mxsih\;\n";
print "//* Created from $infile on $nowstring\n";



exit;




