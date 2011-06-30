{
trackDeDxE1PtSlice1->DrawNormalized();
trackDeDxE1PtSlice2->SetLineColor(2);
trackDeDxE1PtSlice2->DrawNormalized("same");
trackDeDxE1PtSlice3->SetLineColor(3);
trackDeDxE1PtSlice3->DrawNormalized("same");
trackDeDxE1PtSlice4->SetLineColor(4);
trackDeDxE1PtSlice4->DrawNormalized("same");
trackDeDxE1PtSlice5->SetLineColor(6);
trackDeDxE1PtSlice5->DrawNormalized("same");
trackDeDxE1PtSlice6->SetLineColor(7);
trackDeDxE1PtSlice6->DrawNormalized("same");
trackDeDxE1PtSlice7->SetLineColor(8);
trackDeDxE1PtSlice7->DrawNormalized("same");

TPaveText *pt = new TPaveText(.49,.57,.77,.89,"NDC");
pt->AddText("black: 15 < Pt < 25 GeV");
pt->AddText("red: 25 < Pt < 50 GeV");
pt->AddText("light green: 50 < Pt < 75 GeV");
pt->AddText("blue: 75 < Pt < 100 GeV");
pt->AddText("violet: 100 < Pt < 125 GeV");
pt->AddText("teal: 125 < Pt < 150 GeV");
pt->AddText("green: 150 < Pt < 175 GeV");
pt->Draw("same");

}
