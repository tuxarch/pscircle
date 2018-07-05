#include "gtest/gtest.h"

extern "C" {
#include "node.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(node_add, links_are_updated) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};

	node_add(&p, &c1);
	node_add(&p, &c2);
	node_add(&p, &c3);

	ASSERT_EQ(p.first, &c1);
	EXPECT_EQ(c1.next, &c2);
	EXPECT_EQ(c2.next, &c3);
	EXPECT_EQ(c3.next, nullptr);

	ASSERT_EQ(p.last, &c3);
	EXPECT_EQ(c3.prev, &c2);
	EXPECT_EQ(c2.prev, &c1);
	EXPECT_EQ(c1.prev, nullptr);
}

TEST(node_nchildren, large_tree) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};
	node_t c11 = {};
	node_t c21 = {};
	node_t c22 = {};
	node_t c31 = {};
	node_t c32 = {};
	node_t c33 = {};

	node_add(&p, &c1);
	node_add(&p, &c2);
	node_add(&p, &c3);
	node_add(&c1, &c11);
	node_add(&c2, &c21);
	node_add(&c2, &c22);
	node_add(&c3, &c31);
	node_add(&c3, &c32);
	node_add(&c3, &c33);

	EXPECT_EQ(node_nchildren(&p), 3u);
	EXPECT_EQ(node_nchildren(&c1), 1u);
	EXPECT_EQ(node_nchildren(&c2), 2u);
	EXPECT_EQ(node_nchildren(&c3), 3u);
}

TEST(arrange, in_line) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};
	node_t c4 = {};

	node_add(&p, &c1);
	node_add(&c1, &c2);
	node_add(&c2, &c3);
	node_add(&c3, &c4);

	node_arrange(&p);

	EXPECT_NEAR(p.x,  0., EPS);
	EXPECT_NEAR(c1.x, 0., EPS);
	EXPECT_NEAR(c2.x, 0., EPS);
	EXPECT_NEAR(c3.x, 0., EPS);
	EXPECT_NEAR(c4.x, 0., EPS);
}

TEST(arrange, one_level) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};
	node_t c4 = {};

	node_add(&p, &c1);
	node_add(&p, &c2);
	node_add(&p, &c3);
	node_add(&p, &c4);

	node_arrange(&p);

	double n = 1. / 3;
	EXPECT_NEAR(p.x,  n * 1.5, EPS);
	EXPECT_NEAR(c1.x, n * 0., EPS);
	EXPECT_NEAR(c2.x, n * 1., EPS);
	EXPECT_NEAR(c3.x, n * 2., EPS);
	EXPECT_NEAR(c4.x, n * 3., EPS);
}

TEST(arrange, two_levels_a) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};
	node_t c11 = {};
	node_t c21 = {};
	node_t c31 = {};

	node_add(&p, &c1);
	node_add(&p, &c2);
	node_add(&p, &c3);

	node_add(&c1, &c11);
	node_add(&c2, &c21);
	node_add(&c3, &c31);

	node_arrange(&p);

	double n = 1. / 2;
	EXPECT_NEAR(p.x,  n * 1, EPS);
	EXPECT_NEAR(c1.x, n * 0, EPS);
	EXPECT_NEAR(c2.x, n * 1, EPS);
	EXPECT_NEAR(c3.x, n * 2, EPS);
	EXPECT_NEAR(c11.x, n * 0, EPS);
	EXPECT_NEAR(c21.x, n * 1, EPS);
	EXPECT_NEAR(c31.x, n * 2, EPS);
}

TEST(arrange, two_levels_b) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};
	node_t c21 = {};
	node_t c22 = {};
	node_t c23 = {};

	node_add(&p, &c1);
	node_add(&p, &c2);
	node_add(&p, &c3);

	node_add(&c2, &c21);
	node_add(&c2, &c22);
	node_add(&c2, &c23);

	node_arrange(&p);

	double n = 1. / 2;
	EXPECT_NEAR(p.x,  n * 1, EPS);
	EXPECT_NEAR(c1.x, n * 0, EPS);
	EXPECT_NEAR(c2.x, n * 1, EPS);
	EXPECT_NEAR(c3.x, n * 2, EPS);
	EXPECT_NEAR(c21.x, n * 0, EPS);
	EXPECT_NEAR(c22.x, n * 1, EPS);
	EXPECT_NEAR(c23.x, n * 2, EPS);
}

TEST(arrange, two_levels_c) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};
	node_t c21 = {};
	node_t c22 = {};
	node_t c23 = {};
	node_t c31 = {};
	node_t c32 = {};

	node_add(&p, &c1);
	node_add(&p, &c2);
	node_add(&p, &c3);

	node_add(&c2, &c21);
	node_add(&c2, &c22);
	node_add(&c2, &c23);

	node_add(&c3, &c31);
	node_add(&c3, &c32);

	node_arrange(&p);

	double n = 1. / 4;
	EXPECT_NEAR(p.x,  n * 1.75, EPS);
	EXPECT_NEAR(c1.x, n * 0, EPS);
	EXPECT_NEAR(c2.x, n * 1, EPS);
	EXPECT_NEAR(c3.x, n * 3.5, EPS);
	EXPECT_NEAR(c21.x, n * 0, EPS);
	EXPECT_NEAR(c22.x, n * 1, EPS);
	EXPECT_NEAR(c23.x, n * 2, EPS);
	EXPECT_NEAR(c31.x, n * 3, EPS);
	EXPECT_NEAR(c32.x, n * 4, EPS);
}

TEST(arrange, three_levels_a) {
	node_t p = {};
	node_t c[6] = {};
	node_t c0[7] = {};
	node_t c060 = {};
	node_t c30 = {};
	node_t c50 = {};
	node_t c50_[7] = {};

	for (size_t i = 0; i < 6; ++i)
		node_add(&p, &c[i]);
	for (size_t i = 0; i < 7; ++i)
		node_add(&c[0], &c0[i]);
	node_add(&c0[6], &c060);
	node_add(&c[3], &c30);
	node_add(&c[5], &c50);
	for (size_t i = 0; i < 7; ++i)
		node_add(&c50, &c50_[i]);

	node_arrange(&p);

	double n = 1. / 13;
	EXPECT_NEAR(p.x,  n * 6.5, EPS);
	EXPECT_NEAR(c[0].x, n * 3, EPS);
	EXPECT_NEAR(c[1].x, n * 4.53333333333333333, EPS);
	EXPECT_NEAR(c[2].x, n * 6.06666666666666666, EPS);
	EXPECT_NEAR(c[3].x, n * 7.6, EPS);
	EXPECT_NEAR(c[4].x, n * 8.8, EPS);
	EXPECT_NEAR(c[5].x, n * 10, EPS);
	EXPECT_NEAR(c0[0].x, n * 0, EPS);
	EXPECT_NEAR(c0[1].x, n * 1, EPS);
	EXPECT_NEAR(c0[2].x, n * 2, EPS);
	EXPECT_NEAR(c0[3].x, n * 3, EPS);
	EXPECT_NEAR(c0[4].x, n * 4, EPS);
	EXPECT_NEAR(c0[5].x, n * 5, EPS);
	EXPECT_NEAR(c0[6].x, n * 6, EPS);
	EXPECT_NEAR(c060.x, n * 6, EPS);
	EXPECT_NEAR(c30.x, n * 7.6, EPS);
	EXPECT_NEAR(c50.x, n * 10, EPS);
	EXPECT_NEAR(c50_[0].x, n * 7, EPS);
	EXPECT_NEAR(c50_[1].x, n * 8, EPS);
	EXPECT_NEAR(c50_[2].x, n * 9, EPS);
	EXPECT_NEAR(c50_[3].x, n * 10, EPS);
	EXPECT_NEAR(c50_[4].x, n * 11, EPS);
	EXPECT_NEAR(c50_[5].x, n * 12, EPS);
	EXPECT_NEAR(c50_[6].x, n * 13, EPS);
}

TEST(arrange, four_levels_a) {
	node_t r = {};
	node_t r1 = {}, r2 = {}, r3 = {}, r4 = {};
	node_t r11 = {}, r111 = {}, r1111 = {};
	node_t r21 = {}, r211 = {};
	node_t r31 = {}, r311 = {};
	node_t r41 = {};
	node_t r41_[15] = {};

	node_add(&r, &r1);
	node_add(&r, &r2);
	node_add(&r, &r3);
	node_add(&r, &r4);
	node_add(&r1, &r11);
	node_add(&r11, &r111);
	node_add(&r111, &r1111);
	node_add(&r2, &r21);
	node_add(&r21, &r211);
	node_add(&r3, &r31);
	node_add(&r31, &r311);
	node_add(&r4, &r41);

	for (size_t i = 0; i < 15; ++i)
		node_add(&r41, &r41_[i]);

	node_arrange(&r);

	double n = 1. / 17;
	EXPECT_NEAR(r.x,  n * 5, EPS);
	EXPECT_NEAR(r1.x,  n * 0, EPS);
	EXPECT_NEAR(r2.x,  n * 1, EPS);
	EXPECT_NEAR(r3.x,  n * 2, EPS);
	EXPECT_NEAR(r4.x,  n * 10, EPS);
	EXPECT_NEAR(r11.x,  n * 0, EPS);
	EXPECT_NEAR(r111.x,  n * 0, EPS);
	EXPECT_NEAR(r1111.x,  n * 0, EPS);
	EXPECT_NEAR(r21.x,  n * 1, EPS);
	EXPECT_NEAR(r211.x,  n * 1, EPS);
	EXPECT_NEAR(r31.x,  n * 2, EPS);
	EXPECT_NEAR(r311.x,  n * 2, EPS);
	EXPECT_NEAR(r41.x,  n * 10, EPS);
	for (size_t i = 0; i < 15; ++i)
		EXPECT_NEAR(r41_[i].x,  n * (i+3), EPS);
}

TEST(node_reorder_by_leaves, single_level) {
	node_t r = {};
	static const size_t N = 5;
	node_t c[N] = {};

	for (size_t i = 0; i < N; ++i)
		node_add(&r, &c[i]);

	node_reorder_by_leaves(&r);

	for (size_t i = 0; i < N; ++i) {
		bool found = false;
		for (auto n = r.first; n; n = n->next) {
			if (&c[i] == n)
				found = true;
		}
		ASSERT_EQ(found, true);
	}
}

TEST(node_reorder_by_leaves, single_child) {
	node_t r = {};
	static const size_t N = 5;
	node_t c[N] = {};
	node_t c2[N] = {};

	for (size_t i = 0; i < N; ++i) {
		node_add(&r, &c[i]);
		node_add(&c[i], &c2[i]);
	}

	node_reorder_by_leaves(&r);

	for (size_t i = 0; i < N; ++i) {
		bool found = false;
		for (auto n = r.first; n; n = n->next) {
			if (&c[i] == n)
				found = true;
		}
		ASSERT_EQ(found, true);
		EXPECT_EQ(c[i].first, &c2[i]);
		EXPECT_EQ(c[i].first->next, nullptr);
	}
}


TEST(node_reorder_by_leaves, two_levels_odd) {
	node_t r = {};
	static const size_t N = 5;
	node_t c1[N] = {};
	node_t c2[N][N] = {};

	for (size_t i = 0; i < N; ++i) {
		node_add(&r, &c1[i]);
		for (size_t j = 0; j <= i; ++j) {
			node_add(&c1[i], &c2[i][j]);
		}
	}

	node_reorder_by_leaves(&r);

	node_t *n = r.first;
	for (int i : {0, 2, 4, 3, 1}) {
		EXPECT_EQ(n, &c1[i]);
		n = n->next;
	}
}

TEST(node_reorder_by_leaves, two_elements) {
	node_t r = {};
	static const size_t N = 2;
	node_t c1[N] = {};
	node_t c2[N][N] = {};

	for (size_t i = 0; i < N; ++i) {
		node_add(&r, &c1[i]);
		for (size_t j = 0; j <= i; ++j) {
			node_add(&c1[i], &c2[i][j]);
		}
	}

	node_reorder_by_leaves(&r);

	node_t *n = r.first;
	for (int i : {0, 1}) {
		EXPECT_EQ(n, &c1[i]);
		n = n->next;
	}
}

TEST(node_reorder_by_leaves, two_levels_even) {
	node_t r = {};
	static const size_t N = 6;
	node_t c1[N] = {};
	node_t c2[N][N] = {};

	for (size_t i = 0; i < N; ++i) {
		node_add(&r, &c1[i]);
		for (size_t j = 0; j <= i; ++j) {
			node_add(&c1[i], &c2[i][j]);
		}
	}

	node_reorder_by_leaves(&r);

	auto n = r.first;
	for (int i : {0, 2, 4, 5, 3, 1}) {
		EXPECT_EQ(n, &c1[i]);

		n = n->next;
	}
}

TEST(node_widest_child, two_levels) {
	node_t p = {};
	node_t c1 = {};
	node_t c2 = {};
	node_t c3 = {};
	node_t c21 = {};
	node_t c22 = {};
	node_t c23 = {};
	node_t c24 = {};
	node_t c31 = {};
	node_t c32 = {};

	node_add(&p, &c1);
	node_add(&p, &c2);
	node_add(&p, &c3);

	node_add(&c2, &c21);
	node_add(&c2, &c22);
	node_add(&c2, &c23);
	node_add(&c2, &c24);

	node_add(&c3, &c31);
	node_add(&c3, &c32);

	node_arrange(&p);

	EXPECT_EQ(node_widest_child(&p), &c2);
}
