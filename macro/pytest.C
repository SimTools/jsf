{
// 
// Run Pythia test program.  To run,
//  $ jsf -b -q pytest.C
//

  TPythia *py=new TPythia();

  py->PyTest(1);
}
