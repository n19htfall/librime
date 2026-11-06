//
// Copyright RIME Developers
// Distributed under the BSD License
//

#include <gtest/gtest.h>
#include <rime/candidate.h>
#include <rime/common.h>
#include <rime/composition.h>
#include <rime/menu.h>
#include <rime/translation.h>

using namespace rime;

static an<Menu> menu_of(an<Candidate> cand) {
  auto menu = New<Menu>();
  menu->AddTranslation(New<UniqueTranslation>(cand));
  return menu;
}

// Candidates created by scripts may report a range that does not match the
// segment they are listed in. Rendering the composition must not throw
// std::out_of_range, which would escape the C API and abort the frontend.
TEST(RimeCompositionTest, CandidateRangeBeyondInput) {
  Composition comp;
  comp.Reset("nl7yq");
  Segment s0(0, 2);
  s0.status = Segment::kSelected;
  s0.menu = menu_of(New<SimpleCandidate>("phrase", 0, 2, "A"));
  Segment s1(2, 3);
  s1.status = Segment::kGuess;
  // ends past the input
  s1.menu = menu_of(New<SimpleCandidate>("abbrev", 3, 6, "B"));
  Segment s2(3, 5);
  comp.push_back(s0);
  comp.push_back(s1);
  comp.push_back(s2);

  Preedit preedit;
  EXPECT_NO_THROW(preedit = comp.GetPreedit("nl7yq", 5, ""));
  EXPECT_EQ("AB", preedit.text);
  EXPECT_NO_THROW(comp.GetScriptText());
  EXPECT_NO_THROW(comp.GetCommitText());
  EXPECT_NO_THROW(comp.GetCandidatePreview("nl7yq"));
}

TEST(RimeCompositionTest, RawSegmentAfterOverlongCandidate) {
  Composition comp;
  comp.Reset("abcd");
  Segment s0(0, 2);
  s0.status = Segment::kSelected;
  // claims to cover the whole input although its segment ends at 2
  s0.menu = menu_of(New<SimpleCandidate>("phrase", 0, 5, "X"));
  Segment s1(2, 4);
  s1.status = Segment::kGuess;
  Segment s2(4, 4);
  comp.push_back(s0);
  comp.push_back(s1);
  comp.push_back(s2);

  Preedit preedit;
  EXPECT_NO_THROW(preedit = comp.GetPreedit("abcd", 4, ""));
  EXPECT_EQ("X", preedit.text);
  string script_text;
  EXPECT_NO_THROW(script_text = comp.GetScriptText());
  EXPECT_EQ("X", script_text);
}
