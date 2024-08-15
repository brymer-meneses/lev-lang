#include "source_metadata.h"

#include <gtest/gtest.h>

TEST(Source, SourceMetadata) {
  Lev::SourceMetadata metadata{};

  metadata.RegisterLineInfo(Lev::BufferPosition(0, 12));
  metadata.RegisterLineInfo(Lev::BufferPosition(12, 20));

  EXPECT_EQ(0, metadata.GetLineNumber(8));
  EXPECT_EQ(1, metadata.GetLineNumber(13));

  EXPECT_EQ(11, metadata.GetLineColumnOffset(11));
  EXPECT_EQ(1, metadata.GetLineColumnOffset(13));
  EXPECT_EQ(0, metadata.GetLineColumnOffset(20));

  EXPECT_EQ(Lev::LinePosition(0, 5, 0),
            metadata.ConvertToLinePosition(Lev::BufferPosition(0, 5)));

  EXPECT_EQ(Lev::LinePosition(0, 3, 1),
            metadata.ConvertToLinePosition(Lev::BufferPosition(12, 15)));
}
