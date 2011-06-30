{
  TkDeDxMatchedEstPSlices->cd();

  trackDeDxMatchedE1PSlice1->DrawNormalized();
  trackDeDxMatchedE1PSlice2->SetLineColor(2);
  trackDeDxMatchedE1PSlice2->DrawNormalized("same");
  trackDeDxMatchedE1PSlice3->SetLineColor(3);
  trackDeDxMatchedE1PSlice3->DrawNormalized("same");
  trackDeDxMatchedE1PSlice4->SetLineColor(4);
  trackDeDxMatchedE1PSlice4->DrawNormalized("same");
  trackDeDxMatchedE1PSlice5->SetLineColor(6);
  trackDeDxMatchedE1PSlice5->DrawNormalized("same");
  trackDeDxMatchedE1PSlice6->SetLineColor(7);
  trackDeDxMatchedE1PSlice6->DrawNormalized("same");
  trackDeDxMatchedE1PSlice7->SetLineColor(8);
  trackDeDxMatchedE1PSlice7->DrawNormalized("same");
  trackDeDxMatchedE1PSlice8->SetLineColor(29);
  trackDeDxMatchedE1PSlice8->DrawNormalized("same");
  trackDeDxMatchedE1PSlice9->SetLineColor(46);
  trackDeDxMatchedE1PSlice9->DrawNormalized("same");
  //trackDeDxMatchedE1PSlice1->Draw();
  //trackDeDxMatchedE1PSlice2->SetLineColor(2);
  //trackDeDxMatchedE1PSlice2->Draw("same");
  //trackDeDxMatchedE1PSlice3->SetLineColor(3);
  //trackDeDxMatchedE1PSlice3->Draw("same");
  //trackDeDxMatchedE1PSlice4->SetLineColor(4);
  //trackDeDxMatchedE1PSlice4->Draw("same");
  //trackDeDxMatchedE1PSlice5->SetLineColor(6);
  //trackDeDxMatchedE1PSlice5->Draw("same");
  //trackDeDxMatchedE1PSlice6->SetLineColor(7);
  //trackDeDxMatchedE1PSlice6->Draw("same");
  //trackDeDxMatchedE1PSlice7->SetLineColor(8);
  //trackDeDxMatchedE1PSlice7->Draw("same");
  //trackDeDxMatchedE1PSlice8->SetLineColor(29);
  //trackDeDxMatchedE1PSlice8->Draw("same");
  //trackDeDxMatchedE1PSlice9->SetLineColor(46);
  //trackDeDxMatchedE1PSlice9->Draw("same");

  TLegend *pt = new TLegend(.49,.57,.77,.89);
  pt->AddEntry("trackDeDxMatchedE1PSlice1","15 < P < 25 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice2","25 < P < 50 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice3","50 < P < 75 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice4","75 < P < 100 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice5","100 < P < 125 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice6","125 < P < 150 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice7","150 < P < 175 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice8","175 < P < 200 GeV","l");
  pt->AddEntry("trackDeDxMatchedE1PSlice9","200 < P < 300 GeV","l");

  pt->Draw("same");

}
