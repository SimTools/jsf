#!/usr/local/bin/perl
##################################################################
#
#  To create Matrix library with double precission 
#  types.
#  This script reads a source file of Root distribution
#  and modify source files so as to use double precission variable
#  for matrix element.
#  (How to use)
#   1) Change $rootsrcdir in the following line properly to 
#      point directories where source files for libMatrix are.
#   2) Then execute 
#      perl mkmatrix
#   3) Souce files such as JSFDMatrix, etc are created in src directory
#   4) do make to build libJSFDMatrix.so and jsf application.
#
#  Note that, in the standard JSF distribution, JSFDMatrix library
#  created from ROOT.2.23/11 are included.  If you have problem in using
#  JSFDMatrix, try to re-create JSFDMatrix source files using the
#  ROOT source files corresponding to you.
#
#  (Author)
#   Akiya Miyamoto  3-Feburary-2000
#
#$ID$
#
##################################################################

$rootsrcdir="/opt/src/root_v2.23.11/src";
$destdir="../src/";


$MATRIXH       = "TVector.h TMatrix.h TMatrixUtils.h";
$MATRIXS       = "MATRIX_MatrixUtils.cxx MATRIX_Matrix.cxx MATRIX_Vector.cxx";
$MATRIXL       = "MATRIX_LinkDef.h";
#$MATRIXDS      = "MATRIX_G__Matrix.cxx";

$filelist=join(" ",$MATRIXH, $MATRIXS,$MATRIXL);
#$filelist=join(" ",$MATRIXH);

@files=split(" ",$filelist);

foreach $f ( @files ) {
  @type=split(".",$f);

  $outf=$f;
  $outf =~ s/^MATRIX_LinkDef/JSFDMatrixLinkDef/;
#  $outf =~ s/^MATRIX_G__Matrix/libJSFDMatrix/;
  $outf =~ s/^MATRIX_/T/;
  $outf =~ s/^TMatrix/JSFDMatrix/;
  $outf =~ s/^TVector/JSFDVector/;

  print "$f to $outf\n";

  open(FIN,"cat $rootsrcdir/$f|");
  system("rm -f $destdir/$outf");
  open(FOUT,"> $destdir/$outf");
  while(<FIN>) {
    if ( $_ !~ /^\/\// ) {
      s/TMatrix/JSFDMatrix/g;
      s/TVector/JSFDVector/g;
      s/Real_t/Double_t/g;
      s/TElementAction/JSFDElementAction/g;
      s/TElementPosAction/JSFDElementPosAction/g;
      s/TLazyMatrix/JSFDLazyMatrix/g;
      s/THaarMatrix/JSFDHaarMatrix/g;
#      s/G__Matrix/libJSFDMatrix/g;
    }
    print FOUT ;
  }
  close(FIN);
  close(FOUT);


}


