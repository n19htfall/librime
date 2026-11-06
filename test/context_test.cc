//
// Copyright RIME Developers
// Distributed under the BSD License
//

#include <gtest/gtest.h>
#include <rime/candidate.h>
#include <rime/common.h>
#include <rime/context.h>
#include <rime/menu.h>
#include <rime/translation.h>

using namespace rime;

TEST(RimeContextTest, DeleteCandidateOutOfRange) {
  Context ctx;
  ctx.set_input("ab");
  Segment seg(0, 2);
  seg.menu = New<Menu>();
  seg.menu->AddTranslation(
      New<UniqueTranslation>(New<SimpleCandidate>("phrase", 0, 2, "X")));
  ctx.composition().push_back(seg);

  int deletions = 0;
  ctx.delete_notifier().connect([&](Context*) { ++deletions; });

  // there is only one candidate; index 5 does not exist.
  EXPECT_FALSE(ctx.DeleteCandidate(5));
  EXPECT_EQ(0, deletions);
  EXPECT_EQ(0, ctx.composition().back().selected_index);

  EXPECT_TRUE(ctx.DeleteCandidate(0));
  EXPECT_EQ(1, deletions);
}

TEST(RimeContextTest, DeleteCandidateWithoutMenu) {
  Context ctx;
  ctx.set_input("ab");
  ctx.composition().push_back(Segment(0, 2));
  int deletions = 0;
  ctx.delete_notifier().connect([&](Context*) { ++deletions; });
  EXPECT_FALSE(ctx.DeleteCurrentSelection());
  EXPECT_EQ(0, deletions);
}
