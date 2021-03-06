#include <obs.h>

struct test_filter {
	obs_source_t source;
	effect_t whatever;
};

static const char *filter_getname(const char *locale)
{
	UNUSED_PARAMETER(locale);
	return "Test";
}

static void filter_destroy(void *data)
{
	struct test_filter *tf = data;

	if (tf) {
		gs_entercontext(obs_graphics());

		effect_destroy(tf->whatever);
		bfree(tf);

		gs_leavecontext();
	}
}

static void *filter_create(obs_data_t settings, obs_source_t source)
{
	struct test_filter *tf = bzalloc(sizeof(struct test_filter));
	char *effect_file;

	gs_entercontext(obs_graphics());

	effect_file = obs_find_plugin_file("test-input/test.effect");

	tf->source = source;
	tf->whatever = gs_create_effect_from_file(effect_file, NULL);
	bfree(effect_file);
	if (!tf->whatever) {
		filter_destroy(tf);
		return NULL;
	}

	gs_leavecontext();

	UNUSED_PARAMETER(settings);
	return tf;
}

static void filter_render(void *data, effect_t effect)
{
	struct test_filter *tf = data;
	obs_source_process_filter(tf->source, tf->whatever, 0, 0, GS_RGBA,
			ALLOW_DIRECT_RENDERING);

	UNUSED_PARAMETER(effect);
}

struct obs_source_info test_filter = {
	.id           = "test_filter",
	.type         = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.getname      = filter_getname,
	.create       = filter_create,
	.destroy      = filter_destroy,
	.video_render = filter_render
};
