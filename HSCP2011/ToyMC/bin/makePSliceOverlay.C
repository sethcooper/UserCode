{
trackDeDxE1PSlice1->DrawNormalized();
trackDeDxE1PSlice2->SetLineColor(2);
trackDeDxE1PSlice2->DrawNormalized("same");
trackDeDxE1PSlice3->SetLineColor(3);
trackDeDxE1PSlice3->DrawNormalized("same");
trackDeDxE1PSlice4->SetLineColor(4);
trackDeDxE1PSlice4->DrawNormalized("same");
trackDeDxE1PSlice5->SetLineColor(6);
trackDeDxE1PSlice5->DrawNormalized("same");
trackDeDxE1PSlice6->SetLineColor(7);
trackDeDxE1PSlice6->DrawNormalized("same");
trackDeDxE1PSlice7->SetLineColor(8);
trackDeDxE1PSlice7->DrawNormalized("same");

TPaveText *pt = new TPaveText(.49,.57,.77,.89,"NDC");
pt->AddText("black: 15 < P < 25 GeV");
pt->AddText("red: 25 < P < 50 GeV");
pt->AddText("light green: 50 < P < 75 GeV");
pt->AddText("blue: 75 < P < 100 GeV");
pt->AddText("violet: 100 < P < 125 GeV");
pt->AddText("teal: 125 < P < 150 GeV");
pt->AddText("green: 150 < P < 175 GeV");
pt->Draw("same");

}
