//  
//  An example to run jetnet example, JNTDEC.
//  To run,
//     $  jsf jntdec.C
//  jobout.jntdec is a sample output.
//

Int_t jntdec_orig()
{
  gSystem->Load("libJSFJETNET.so");
  JSFJETNET::JNTDEC(0);
}



