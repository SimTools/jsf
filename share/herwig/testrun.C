{
  gSystem->Load("libEG.so");
  gSystem->Load("libTHerwig.so");

  THerwig herwig;

  herwig.SetBeams("PBAR","P",1000.0, 1000.0, 1500);
  
  herwig.Initialize();

  for(Int_t i=0;i<10;i++){
    herwig.GenerateEvent();

    cout << "NHEP=" << herwig.GetNHEP() << endl;
    
    //    cout << "Number of particles is " << herwig.GetNumberOfParticles() << endl;



  }

  herwig.Terminate();

}
