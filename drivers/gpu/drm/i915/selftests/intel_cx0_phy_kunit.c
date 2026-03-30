// SPDX-License-Identifier: GPL-2.0
#include <kunit/test.h>

#include "display/intel_cx0_phy_test.h"

/* forward declare opaque types used in prototypes */
struct intel_c10pll_state;
struct intel_c20pll_state;

/* forward decls from production code */
int intel_c10pll_calc_port_clock(const struct intel_c10pll_state *pll);
int intel_c20pll_calc_port_clock(const struct intel_c20pll_state *pll);
bool intel_dpll_clock_matches(int clock, int target);

/* If struct intel_cx0pll_params isn't visible via a header, keep your local copy */
struct intel_cx0pll_params {
	const char *name;
	int clock_rate;
	bool is_c10;
	union {
		const struct intel_c10pll_state *c10;
		const struct intel_c20pll_state *c20;
	};
};

static void check_tables(struct kunit *test, const struct intel_cx0pll_params *tables)
{
	int i;

	for (i = 0; tables[i].name; i++) {
		int expected = tables[i].clock_rate;
		int actual = intel_cx0_phy_test_calc_port_clock(&tables[i]);

		KUNIT_EXPECT_TRUE_MSG(test,
				      intel_dpll_clock_matches(actual, expected),
				      "%s: computed=%d expected=%d (is_c10=%d)\n",
				      tables[i].name, actual, expected, tables[i].is_c10);
	}
}

static void intel_cx0_phy_pll_tables(struct kunit *test)
{
	check_tables(test, intel_cx0_phy_get_mtl_c10_edp_tables());
	check_tables(test, intel_cx0_phy_get_mtl_c10_dp_tables());
	check_tables(test, intel_cx0_phy_get_mtl_c10_hdmi_tables());

	check_tables(test, intel_cx0_phy_get_xe2hpd_c20_edp_tables());
	check_tables(test, intel_cx0_phy_get_mtl_c20_dp_tables());
	check_tables(test, intel_cx0_phy_get_xe2hpd_c20_dp_tables());
	check_tables(test, intel_cx0_phy_get_xe3lpd_c20_dp_edp_tables());
	check_tables(test, intel_cx0_phy_get_mtl_c20_hdmi_tables());
}

static struct kunit_case intel_cx0_phy_test_cases[] = {
	KUNIT_CASE(intel_cx0_phy_pll_tables),
	{}
};

static struct kunit_suite intel_cx0_phy_test_suite = {
	.name = "intel-cx0-phy",
	.test_cases = intel_cx0_phy_test_cases,
};

kunit_test_suite(intel_cx0_phy_test_suite);
