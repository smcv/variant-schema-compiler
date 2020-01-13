#include "sample.h"

void
test_sample_variant (GVariant *v)
{
  SampleTestRef t;
  SampleVarRef var;
  GVariant *varv;
  const char *res;
  SampleVariantRef resv;

  g_print ("sample type: %s\n", g_variant_get_type_string (v));
  g_print ("sample: %s\n", g_variant_print (v, FALSE));
  g_print ("sample with types: %s\n", g_variant_print (v, TRUE));

  g_assert (g_variant_type_equal (g_variant_get_type(v), SAMPLE_TEST_TYPEFORMAT));

  t = sample_test_ref_from_variant (v);
  g_print ("custom: %s\n", sample_test_ref_print (t, FALSE));
  g_print ("custom with types: %s\n", sample_test_ref_print (t, TRUE));

  g_assert_cmpstr (g_variant_print (v, FALSE), ==, sample_test_ref_print (t, FALSE));
  g_assert_cmpstr (g_variant_print (v, TRUE), ==, sample_test_ref_print (t, TRUE));

  var = sample_var_ref_from_variant_ref(sample_test_ref_get_v(t));
  varv = g_variant_get_variant (g_variant_get_child_value (v, SAMPLE_TEST_INDEXOF_V));
  g_assert_cmpstr (g_variant_print (varv, TRUE), ==, sample_var_ref_print (var, TRUE));

  SampleD2Ref d2 = sample_test_ref_get_d2(t);
  GVariant *v3 = sample_d2_ref_dup_to_variant(d2);
  g_assert_cmpstr (g_variant_print (v3, TRUE), ==, sample_d2_ref_print(d2, FALSE));

  g_assert (sample_d2_ref_lookup(d2, 1, &res));
  g_assert_cmpstr (res, ==, "a");
  g_assert (sample_d2_ref_lookup(d2, 3, &res));
  g_assert_cmpstr (res, ==, "b");
  g_assert (!sample_d2_ref_lookup(d2, 2, &res));

  SampleMetadataRef meta = sample_test_ref_get_meta(t);
  GVariant *meta_v = sample_metadata_ref_dup_to_variant(meta);

  g_assert (g_variant_type_equal (g_variant_get_type(meta_v), SAMPLE_METADATA_TYPEFORMAT));
  g_assert_cmpstr (g_variant_print (meta_v, FALSE), ==, sample_metadata_ref_print (meta, FALSE));
  g_assert_cmpstr (g_variant_print (meta_v, TRUE), ==, sample_metadata_ref_print (meta, TRUE));

  g_assert (sample_metadata_ref_lookup(meta, "foo", &resv));
  g_assert_cmpstr ("<1>", ==, sample_variant_ref_print (resv, TRUE));
  g_assert (sample_metadata_ref_lookup(meta, "bar", &resv));
  g_assert_cmpstr ("<'s'>", ==, sample_variant_ref_print (resv, TRUE));
  g_assert (!sample_metadata_ref_lookup(meta, "missing", &resv));
}

int
main (int argc,
      char *argv[])
{
  GVariant *v;

#define DATA \
  "([32, 22], '%s', uint16 16, "                                        \
    "('s2', 322), ('ssss2', 3222), (323,), 324, "                       \
    "<(int16 67, 1023, byte 3, (uint16 5, byte 6))>, "                                          \
    "[(int16 68, 1025, byte 42, (uint16 7, byte 8)), (int16 69, 1026, byte 42, (uint16 9, byte 11))]"                              \
    ", {1:2, 3:4}, {'foo': <1>, 'bar': <'s'>}, {1:'a', 3:'b'}, "        \
    "just (objectpath '/', signature 's', true, handle 3, int64 88, uint64 89, 3.1415 )"             \
    ")"

  v = g_variant_new_parsed (g_strdup_printf (DATA, "s"));
  test_sample_variant(v);

  /* Try with larger offsets */
  v = g_variant_new_parsed (g_strdup_printf (DATA, "sxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
  test_sample_variant(v);
}
