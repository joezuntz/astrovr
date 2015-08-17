

class ColorMap
{
protected:
	float xmin, xmax, xrange;
	bool islog;
public:
	ColorMap(float x_min, float x_max, bool is_log);
	virtual float operator()(float x, float c[3]) = 0;

	/* data */
};


class JetColorMap : public ColorMap {
	public:
	JetColorMap(float x_min, float x_max, bool is_log);
	float operator()(float x, float c[3]);
};