

//----------------------------------------------------------------------------------------------------------------------
template<typename TriVariateFn>
inline void sdf::discretized_field::fill(const TriVariateFn& i_function)
{
	// Use at least boost::thread!
	const float xscale = 1.f/((float)m_grid.width()-1);
	const float yscale = 1.f/((float)m_grid.height()-1);
	const float zscale = 1.f/((float)m_grid.depth()-1);
	const vector3d extent = m_max-m_min;
	unsigned int faceid(0);
	distance_record ft;
	for (unsigned int k=0;k<m_grid.depth();k++)
	{
		for (unsigned int j=0;j<m_grid.height();j++)
		{
			for (unsigned int i=0;i<m_grid.width();i++)
			{
				const point3d point(
					(float)i*xscale*extent[0],
					(float)j*yscale*extent[1],
					(float)k*zscale*extent[2]
				);
				m_grid(i,j,k)=i_function(m_min+point,&faceid,&ft);
			}
		}
	}

}

//----------------------------------------------------------------------------------------------------------------------
template<typename TriVariateFn>
inline void sdf::discretized_field::fill_slice(const TriVariateFn& i_function, unsigned int i_slice)
{
	// Use at least boost::thread!
	const float xscale = 1.f/((float)m_grid.width()-1);
	const float yscale = 1.f/((float)m_grid.height()-1);
	const float zscale = 1.f/((float)m_grid.depth()-1);
	const vector3d extent = m_max-m_min;
	unsigned int faceid(0);
	distance_record ft;
	for (unsigned int j=0;j<m_grid.height();j++)
	{
		for (unsigned int i=0;i<m_grid.width();i++)
		{
			const point3d point(
				(float)i*xscale*extent[0],
				(float)j*yscale*extent[1],
				(float)i_slice*zscale*extent[2]
			);
			m_grid(i,j,i_slice)=i_function(m_min+point,&faceid,&ft);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
template<typename TriVariateFn>
inline void sdf::discretized_field::fill_packets(const TriVariateFn& i_function)
{
	const float xscale = 1.f/((float)m_grid.width()-1);
	const float yscale = 1.f/((float)m_grid.height()-1);
	const float zscale = 1.f/((float)m_grid.depth()-1);
	const vector3d extent = m_max-m_min;
	unsigned int faceid(0);
	distance_record ft;

	assert(m_grid.depth()%2==0);
	assert(m_grid.height()%2==0);
	assert(m_grid.width()%2==0);
	int incoherents = 0;
	int coherents = 0;

	for (unsigned int k=0;k<m_grid.depth();k+=2)
	{
		for (unsigned int j=0;j<m_grid.height();j+=2)
		{
			for (unsigned int i=0;i<m_grid.width();i+=2)
			{
				const point3d points[8] = { 
					m_min+point3d((float)(i+0)*xscale*extent[0],(float)(j+0)*yscale*extent[1],(float)(k+0)*zscale*extent[2]),
					m_min+point3d((float)(i+0)*xscale*extent[0],(float)(j+0)*yscale*extent[1],(float)(k+1)*zscale*extent[2]),
					m_min+point3d((float)(i+0)*xscale*extent[0],(float)(j+1)*yscale*extent[1],(float)(k+0)*zscale*extent[2]),
					m_min+point3d((float)(i+0)*xscale*extent[0],(float)(j+1)*yscale*extent[1],(float)(k+1)*zscale*extent[2]),
					m_min+point3d((float)(i+1)*xscale*extent[0],(float)(j+0)*yscale*extent[1],(float)(k+0)*zscale*extent[2]),
					m_min+point3d((float)(i+1)*xscale*extent[0],(float)(j+0)*yscale*extent[1],(float)(k+1)*zscale*extent[2]),
					m_min+point3d((float)(i+1)*xscale*extent[0],(float)(j+1)*yscale*extent[1],(float)(k+0)*zscale*extent[2]),
					m_min+point3d((float)(i+1)*xscale*extent[0],(float)(j+1)*yscale*extent[1],(float)(k+1)*zscale*extent[2])					
				};
				float dists[8] = { 0,0,0,0,0,0,0,0 };
				if (i_function(points,dists))
					coherents++;
				else
					incoherents--;

				m_grid(i+0,j+0,k+0)=dists[0];
				m_grid(i+0,j+0,k+1)=dists[1];
				m_grid(i+0,j+1,k+0)=dists[2];
				m_grid(i+0,j+1,k+1)=dists[3];
				m_grid(i+1,j+0,k+0)=dists[4];
				m_grid(i+1,j+0,k+1)=dists[5];
				m_grid(i+1,j+1,k+0)=dists[6];
				m_grid(i+1,j+1,k+1)=dists[7];
			}
		}
	}

}


//----------------------------------------------------------------------------------------------------------------------
template<typename TriVariateFn>
inline void sdf::discretized_field::fill_mt(const TriVariateFn& i_function)
{
#ifndef SDF_NO_BOOST_THREAD
	boost::thread_group group;
	thread::work_queue wq(m_grid.depth());
	for (unsigned int i=0;i<boost::thread::hardware_concurrency();i++)
	{
		group.create_thread(boost::bind(
			&discretized_field::fill_mt_single<TriVariateFn>,this,i_function,&wq
			));
	}
	group.join_all();
#else
	fill(i_function);
#endif
}


#ifndef SDF_NO_BOOST_THREAD
//----------------------------------------------------------------------------------------------------------------------
template<typename TriVariateFn>
inline void sdf::discretized_field::fill_mt_single(const TriVariateFn& i_function, thread::work_queue* i_wq)
{
	unsigned int slice(0);
	// Grabing a new slice until none are left
	while (i_wq->next_work_id(&slice))
	{
		fill_slice(i_function,slice);
	};
}
#endif
