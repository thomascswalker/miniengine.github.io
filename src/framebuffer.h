#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cfloat>
#include <memory>
#include <typeinfo>
#include <windows.h>
#include <cassert>
#include <sstream>
#include <map>

#include "camera.h"
#include "channel.h"
#include "color.h"
#include "matrix.h"
#include "mesh.h"
#include "printbuffer.h"
#include "shader.h"

namespace Graphics
{

/**
 * @brief Main class for managing displaying to the screen.
*/
    class Framebuffer
    {
        // Window handle
        HWND m_hwnd = HWND();

        int m_width = DEFAULT_WINDOW_WIDTH;
        int m_height = DEFAULT_WINDOW_HEIGHT;
        Rect<int> m_frame;

        // Channels
        std::map<const char*, Channel*> m_channels;

        // Pixel memory
        SIZE_T m_bufferSize = 0;
        void* m_displayBuffer = nullptr;
        BITMAPINFO m_bufferBmi;
        const int m_bytesPerPixel = 4;

        // Vertex memory
        std::vector<Triangle*> m_triangles;

        // Camera and matrices
        Camera m_camera;
        Vector3 m_targetPosition;
        Matrix4 m_view = Matrix4();
        Matrix4 m_proj = Matrix4();
        Matrix4 m_mvp = Matrix4();
        Matrix4 m_model = Matrix4();

        // Grid
        std::vector<Vector3> m_gridPoints;

     public:
        // Constructor
        Framebuffer(HWND hwnd);

        // Destructor
        ~Framebuffer();

        int getWidth()
        {
            return m_width;
        }
        void setWidth(int width)
        {
            m_width = width;
        }
        int getHeight()
        {
            return m_height;
        }
        void setHeight(int height)
        {
            m_height = height;
        }

        /// <summary>
        /// Set the Framebuffer to the given width and height. This also sets all channels
        /// to this new size.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="width">The new width.</param>
        /// <param name="height">The new height.</param>
        template<typename T>
        inline void setSize(T width, T height)
        {
            m_width = (int)width;
            m_height = (int)height;
            m_frame = Rect<int>(0, 0, m_width, m_height);

            for (auto const& [k, c] : m_channels)
            {
                c->setSize((int)width, (int)height);
                c->allocate();
                c->clear();
            }
        }

        /// <summary>
        /// Set the Framebuffer to the given size's width and height. This also sets all channels
        /// to this new size.
        /// </summary>
        /// <param name="size">The new size.</param>
        inline void setSize(Size size)
        {
            setSize(size.width, size.height);
        }

        /// <summary>
        /// Get the pointer to the given channel.
        /// </summary>
        /// <param name="channel"></param>
        /// <returns>The pointer to the channel.</returns>
        inline Channel* getChannel(const char* channel)
        {
            return m_channels[channel];
        }

        // Camera
        Camera* getCamera()
        {
            return &m_camera;
        }

        // Pixel buffer
        HWND getHwnd()
        {
            return m_hwnd;
        }

        int getBufferSize()
        {
            return m_width * m_height * sizeof(unsigned int);
        }

        HBITMAP getBitmap();
        void bindTriangleBuffer(std::vector<Triangle*> data);

        //void setPixelShader(PixelShader* shader) { m_pixelShader = shader; }

        Vector3 getTargetTranslation()
        {
            return m_targetPosition;
        }

        /// <summary>
        /// Sets the current model matrix to the given matrix.
        /// </summary>
        /// <param name="m">The new matrix to set the model to.</param>
        void setModelMatrix(Matrix4& m)
        {
            m_model = m;
        }

        /// <summary>
        /// Returns the view (camera) matrix.
        /// </summary>
        Matrix4 getViewMatrix()
        {
            return m_view;
        }

        /// <summary>
        /// Returns the projection matrix.
        /// </summary>
        Matrix4 getProjectionMatrix()
        {
            return m_proj;
        }

        /// <summary>
        /// Returns the MVP matrix.
        /// </summary>
        Matrix4 getModelViewProjMatrix()
        {
            return m_mvp;
        }

        /// <summary>
        /// Projects the given world position from world-space to screen-space. This assumes the MVP
        /// matrix has already been computed.
        /// </summary>
        /// <param name="v">The vertex to get screens-space coordinates of.</param>
        Vector3 worldToScreen(Vector3* v);

        /// <summary>
        /// Deprojects the given screen coordinates from screen-space to world-space. where X and Y make up the X/Y position on screen and Z
        /// makes up the depth to project.
        /// </summary>
        /// <param name="x">The X screen coordinate.</param>
        /// <param name="y">The Y screen coordinate.</param>
        /// <param name="z">The world depth to project.</param>
        Vector3 screenToWorld(double x, double y, double z);

        /// <summary>
        /// Given a triangle and a screen-space point on the triangle, returns the z-depth
        /// of said point.
        /// </summary>
        /// <param name="v1">First point in the triangle.</param>
        /// <param name="v2">Second point in the triangle.</param>
        /// <param name="v3">Third point in the triangle.</param>
        /// <param name="p">The screen-space point to determine depth for.</param>
        /// <returns>The z-depth value. Higher values mean further away.</returns>
        double getDepth(Vector3* v1, Vector3* v2, Vector3* v3, Vector3* p);

        /// <summary>
        /// Get the bounding box between two points.
        /// </summary>
        Rect<int> getBoundingBox(Vector3* v1, Vector3* v2);

        /// <summary>
        /// Returns the bounding box between three points.
        /// </summary>
        Rect<int> getBoundingBox(Vector3* v1, Vector3* v2, Vector3* v3);

        /// <summary>
        /// Returns the bounding box of the given triangle.
        /// </summary>
        Rect<int> getBoundingBox(Triangle* t)
        {
            Vector3 v1 = t->v1()->getTranslation();
            Vector3 v2 = t->v2()->getTranslation();
            Vector3 v3 = t->v3()->getTranslation();
            return getBoundingBox(&v1, &v2, &v3);
        };

        /// <summary>
        /// Based on Bresenham�s Line Drawing Algorithm.
        /// </summary>
        /// <param name="v1">Point 1 of the line.</param>
        /// <param name="v2">Point 2 of the line.</param>
        /// <returns>Whether the line was drawn or not.</returns>
        bool drawLine(Vector3* v1, Vector3* v2);

        /// <summary>
        /// Given a screen point, draw a circle with the given radius 'r'.
        /// </summary>
        /// <param name="v">The screen point to draw a circle at.</param>
        /// <param name="r">The circle's radius.</param>
        /// <returns>Whether the circle was drawn or not.</returns>
        bool drawCircle(Vector3* v, double r);

        /// <summary>
        /// Renders the given triangle, through its world-space vertices, to the RGB/Z buffer(s).
        /// </summary>
        /// <param name="triangle">The triangle to draw.</param>
        /// <returns>Whether the triangle was drawn on the buffer (screen) or not.</returns>
        bool drawTriangle(Triangle* triangle);

        /// <summary>
        /// Renders all triangles in the scene (triangle buffer).
        /// 1. Clear all channels of memory.
        /// 2. Set Z channel to be filled with the camera's far clip value.
        /// 3. Construct the MVP matrix, given the current camera orientation.
        /// 4. Draw each triangle to the RGB/Z buffers.
        /// </summary>
        void render();

        /// <summary>
        /// Returns the void pointer to the display buffer.
        /// </summary>
        void* getDisplayPtr()
        {
            return m_displayBuffer;
        }

        /// <summary>
        /// Allocates the display memory void pointer by combining the
        /// R, G, and B channels into a single void pointer filled with uint8 for
        /// each channel, with the fourth chunk set to zero.
        ///
        /// Because channel pixels are doubles, we need to convert those to 0 = > 255.
        ///
        /// R | G | B | None
        /// -- - | -- - | -- - | ----
        /// 255 | 128 | 50 | 0
        /// </summary>
        void allocateDisplayPtr();

    };

}

#endif