

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

class PlanckColorMap : public JetColorMap {
	float x1, x2, x3, x4;
	float y1, y2, y3, y4;
	float x1range;
	float x2range;
	float x3range;

	public: 
	PlanckColorMap();
	float operator()(float x, float c[3]);


};