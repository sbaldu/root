/// \file
/// \ingroup tutorial_tree
/// \notebook
/// Illustrates how to use Tree friends:
///   - create a simple TTree
///   - Copy a subset of this TTree to a new TTree
///   - Create a Tree Index
///   - Make a friend TTree
///   - compare two TTrees
///   - Draw a variable from the first tree versus a variable
///     in the friend Tree
///
/// You can run this tutorial with:
/// ~~~
///  root > .x tree109_friend.C  (interpreted via Cling)
///  root > .x tree109_friend.C+ (executed via ACLIC & the native compiler)
/// ~~~
/// or, variants like:
/// ~~~
///  root > .L tree109_friend.C+
///  root > CreateParentTree();
///  root > CreateFriendTree();
///  root > CompareTrees();
///  root > DrawFriend();
/// ~~~
///
/// \macro_output
/// \macro_image
/// \macro_code
///
/// \author Rene Brun


#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TTree.h"

Int_t Run, Event;
Float_t x, y, z;

void CreateParentTree()
{
   // create a simple TTree with 5 branches
   // Two branches ("Run" and "Event") will be used to index the Tree
   auto f = TFile::Open("treeparent.root", "recreate");
   auto T = new TTree("T", "test friend trees");
   T->Branch("Run", &Run, "Run/I");
   T->Branch("Event", &Event, "Event/I");
   T->Branch("x", &x, "x/F");
   T->Branch("y", &y, "y/F");
   T->Branch("z", &z, "z/F");
   TRandom r;
   for (Int_t i=0; i<10000; i++) {
      if (i < 5000)
         Run = 1;
      else
         Run = 2;
      Event = i;
      x = r.Gaus(10, 1);
      y = r.Gaus(20, 2);
      z = r.Landau(2, 1);
      T->Fill();
   }
   T->Print();
   T->Write();
   delete f;
}

void CreateFriendTree()
{
   // Open the file created by CreateParentTree
   // Copy a subset of the TTree into a new TTree
   // (see also tutorials tree110_copy.C, tree111_copy.C and tree112_copy.C)
   // Create an index on the new TTree ("Run", "Event")
   // Write the new TTree (including its index)

   auto f = TFile::Open("treeparent.root");
   auto T = f->Get<TTree>("T");
   auto ff = TFile::Open("treefriend.root", "recreate");
   auto TF = T->CopyTree("z<10");
   TF->SetName("TF");
   TF->BuildIndex("Run", "Event");
   TF->Write();
   TF->Print();
   delete ff;
}

void CompareTrees()
{
   // The two TTrees created above are compared.
   // The subset of entries in the small TTree must be identical
   // to the entries in the original TTree.

   Int_t fRun, fEvent;
   Float_t fx, fy, fz;
   auto f = TFile::Open("treeparent.root");
   auto T  = f->Get<TTree>("T");
   auto ff = TFile::Open("treefriend.root");
   auto TF = ff->Get<TTree>("TF");
   T->SetBranchAddress("Run", &Run);
   T->SetBranchAddress("Event", &Event);
   T->SetBranchAddress("x", &x);
   T->SetBranchAddress("y", &y);
   T->SetBranchAddress("z", &z);
   TF->SetBranchAddress("Run", &fRun);
   TF->SetBranchAddress("Event", &fEvent);
   TF->SetBranchAddress("x", &fx);
   TF->SetBranchAddress("y", &fy);
   TF->SetBranchAddress("z", &fz);
   T->AddFriend(TF);

   Long64_t nentries = T->GetEntries();
   Int_t nok = 0;
   for (Long64_t i=0; i<nentries; i++) {
      T->GetEntry(i);
      if (fRun == Run && fEvent==Event && x==fx && y==fy && z==fz) {
         nok++;
      } else {
         if (TF->GetEntryWithIndex(Run, Event) > 0) {
            if (i <100)
               printf("i=%lld, Run=%d, Event=%d, x=%g, y=%g, z=%g : fRun=%d, fEvent=%d, fx=%g, fy=%g, fz=%g\n",
                      i, Run, Event, x, y, z, fRun, fEvent, fx, fy, fz);
         }
      }
   }
   printf("nok = %d, fentries=%lld\n", nok, TF->GetEntries());

   delete f;
   delete ff;
}

void DrawFriend()
{
   // Draw a scatter plot of variable x in the parent TTree versus
   // the same variable in the subtree.
   // This should produce points along a straight line.

   auto f = TFile::Open("treeparent.root");
   auto T = f->Get<TTree>("T");
   T->AddFriend("TF", "treefriend.root");
   T->Draw("x:TF.x");
}

void tree109_friend()
{
   CreateParentTree();
   CreateFriendTree();
   CompareTrees();
   DrawFriend();
}
