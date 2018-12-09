﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

namespace IST.RemoteTalk
{

    public static partial class Misc
    {
        public const int InvalidID = -1;

        public static string S(IntPtr cstring)
        {
            return cstring == IntPtr.Zero ? "" : Marshal.PtrToStringAnsi(cstring);
        }

        public static string SanitizeFileName(string name)
        {
            var reg = new Regex("[:<>|\\*\\?]");
            return reg.Replace(name, "_");
        }

        public static void Resize<T>(List<T> list, int n) where T : new()
        {
            int cur = list.Count;
            if (n < cur)
                list.RemoveRange(n, cur - n);
            else if (n > cur)
            {
                if (n > list.Capacity)
                    list.Capacity = n;
                int a = n - cur;
                for (int i = 0; i < a; ++i)
                    list.Add(new T());
            }
        }

        public static T GetOrAddComponent<T>(GameObject go) where T : Component
        {
            var ret = go.GetComponent<T>();
            if (ret == null)
                ret = go.AddComponent<T>();
            return ret;
        }

        public static void ForceRepaint()
        {
#if UNITY_EDITOR
            SceneView.RepaintAll();
            UnityEditorInternal.InternalEditorUtility.RepaintAllViews();
#endif
        }
    }


    public static class rtPlugin
    {
        #region internal
        [DllImport("RemoteTalkClient")] static extern IntPtr rtGetVersion();
        #endregion

        static string version
        {
            get { return Misc.S(rtGetVersion()); }
        }
    }


    public enum rtAudioFormat
    {
        Unknown = 0,
        U8,
        S16,
        S24,
        S32,
        F32,
        RawFile = 100,
    }

    public struct rtAudioData
    {
        #region internal
        public IntPtr self;
        [DllImport("RemoteTalkClient")] static extern rtAudioFormat rtAudioDataGetFormat(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern int rtAudioDataGetChannels(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern int rtAudioDataGetFrequency(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern int rtAudioDataGetSampleLength(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern byte rtAudioDataReadSamplesFloat(IntPtr self, float[] dst, int begin, int end);
        [DllImport("RemoteTalkClient")] static extern byte rtAudioDataExportAsWave(IntPtr self, string path);
        #endregion

        public static implicit operator bool(rtAudioData v) { return v.self != IntPtr.Zero; }

        public rtAudioFormat format
        {
            get { return rtAudioDataGetFormat(self); }
        }
        public int frequency
        {
            get { return rtAudioDataGetFrequency(self); }
        }
        public int channels
        {
            get { return rtAudioDataGetChannels(self); }
        }
        public int sampleLength
        {
            get { return rtAudioDataGetSampleLength(self); }
        }

        public void ReadSamples(float[] dst, int begin, int end) { rtAudioDataReadSamplesFloat(self, dst, begin, end); }
        public bool ExportAsWave(string path) { return rtAudioDataExportAsWave(self, path) != 0; }
    }


    [Serializable]
    public struct rtTalkParamFlags
    {
        [SerializeField] public BitFlags bits;
        public bool mute
        {
            get { return bits[0]; }
            set { bits[0] = value; }
        }
        public bool volume
        {
            get { return bits[1]; }
            set { bits[1] = value; }
        }
        public bool speed
        {
            get { return bits[2]; }
            set { bits[2] = value; }
        }
        public bool pitch
        {
            get { return bits[3]; }
            set { bits[3] = value; }
        }
        public bool intonation
        {
            get { return bits[4]; }
            set { bits[4] = value; }
        }
        public bool joy
        {
            get { return bits[5]; }
            set { bits[5] = value; }
        }
        public bool anger
        {
            get { return bits[6]; }
            set { bits[6] = value; }
        }
        public bool sorrow
        {
            get { return bits[7]; }
            set { bits[7] = value; }
        }
        public bool avator
        {
            get { return bits[8]; }
            set { bits[8] = value; }
        }
    }

    [Serializable]
    public struct rtTalkParams
    {
        [SerializeField] public rtTalkParamFlags flags;
        [SerializeField] int m_mute;
        [SerializeField] float m_volume;
        [SerializeField] float m_speed;
        [SerializeField] float m_pitch;
        [SerializeField] float m_intonation;
        [SerializeField] float m_joy;
        [SerializeField] float m_anger;
        [SerializeField] float m_sorrow;
        [SerializeField] int m_avator;

        public static rtTalkParams defaultValue
        {
            get
            {
                return new rtTalkParams {
                    m_mute = 0,
                    m_volume = 1.0f,
                    m_speed = 1.0f,
                    m_pitch = 1.0f,
                    m_intonation = 1.0f,
                };
            }
        }

        public bool mute
        {
            get { return m_mute != 0; }
            set { m_mute = value ? 1 : 0; flags.mute = true; }
        }
        public float volume
        {
            get { return m_volume; }
            set { m_volume = value; flags.volume = true; }
        }
        public float speed
        {
            get { return m_speed; }
            set { m_speed = value; flags.speed = true; }
        }
        public float pitch
        {
            get { return m_pitch; }
            set { m_pitch = value; flags.pitch = true; }
        }
        public float intonation
        {
            get { return m_intonation; }
            set { m_intonation = value; flags.intonation = true; }
        }
        public float joy
        {
            get { return m_joy; }
            set { m_joy = value; flags.joy = true; }
        }
        public float anger
        {
            get { return m_anger; }
            set { m_anger = value; flags.anger = true; }
        }
        public float sorrow
        {
            get { return m_sorrow; }
            set { m_sorrow = value; flags.sorrow = true; }
        }
        public int avator
        {
            get { return m_avator; }
            set { m_avator = value; flags.avator = true; }
        }
    };

    public struct rtAvatorInfo
    {
        #region internal
        public IntPtr self;
        [DllImport("RemoteTalkClient")] static extern int rtAvatorInfoGetID(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern IntPtr rtAvatorInfoGetName(IntPtr self);
        #endregion

        public static implicit operator bool(rtAvatorInfo v) { return v.self != IntPtr.Zero; }

        public int id { get { return rtAvatorInfoGetID(self); } }
        public string name { get { return Misc.S(rtAvatorInfoGetName(self)); } }
    }

    public struct rtAsync
    {
        #region internal
        public IntPtr self;
        [DllImport("RemoteTalkClient")] static extern byte rtAsyncIsFinished(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern void rtAsyncWait(IntPtr self);
        #endregion

        public static implicit operator bool(rtAsync v) { return v.self != IntPtr.Zero; }
        public void Release() { self = IntPtr.Zero; }

        public bool isFinished { get { return rtAsyncIsFinished(self) != 0; } }
        public void Wait() { rtAsyncWait(self); }
    }


    public delegate void rtAudioDataCallback(rtAudioData curve);

    public struct rtHTTPClient
    {
        #region internal
        public IntPtr self;
        [DllImport("RemoteTalkClient")] static extern rtHTTPClient rtHTTPClientCreate(string server, int port);
        [DllImport("RemoteTalkClient")] static extern void rtHTTPClientRelease(IntPtr self);

        [DllImport("RemoteTalkClient")] static extern rtAsync rtHTTPClientUpdateServerStatus(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern void rtHTTPClientGetParams(IntPtr self, ref rtTalkParams st);
        [DllImport("RemoteTalkClient")] static extern int rtHTTPClientGetNumAvators(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern rtAvatorInfo rtHTTPClientGetAvator(IntPtr self, int i);

        [DllImport("RemoteTalkClient")] static extern byte rtHTTPClientIsReady(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern rtAsync rtHTTPClientTalk(IntPtr self, ref rtTalkParams p, string t);
        [DllImport("RemoteTalkClient")] static extern rtAsync rtHTTPClientStop(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern rtAudioData rtHTTPClientSyncBuffers(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern rtAudioData rtHTTPClientGetBuffer(IntPtr self);
        #endregion

        public static implicit operator bool(rtHTTPClient v) { return v.self != IntPtr.Zero; }

        public rtTalkParams serverParams
        {
            get
            {
                var ret = default(rtTalkParams);
                rtHTTPClientGetParams(self, ref ret);
                return ret;
            }
        }

        public AvatorInfo[] avatorList
        {
            get
            {
                var ret = new AvatorInfo[rtHTTPClientGetNumAvators(self)];
                for (int i = 0; i < ret.Length; ++i)
                {
                    var ai = rtHTTPClientGetAvator(self, i);
                    ret[i] = new AvatorInfo { id = ai.id, name = ai.name };
                }
                return ret;
            }
        }

        public bool isReady
        {
            get { return rtHTTPClientIsReady(self) != 0; }
        }
        public rtAudioData buffer
        {
            get { return rtHTTPClientGetBuffer(self); }
        }

        public static rtHTTPClient Create(string server, int port) { return rtHTTPClientCreate(server, port); }
        public void Release() { rtHTTPClientRelease(self); self = IntPtr.Zero; }

        public rtAsync UpdateServerStatus() { return rtHTTPClientUpdateServerStatus(self); }
        public rtAsync Talk(ref rtTalkParams para, string text) { return rtHTTPClientTalk(self, ref para, text); }
        public rtAsync Stop() { return rtHTTPClientStop(self); }
        public rtAudioData SyncBuffers() { return rtHTTPClientSyncBuffers(self); }
    }


    public struct rtHTTPReceiver
    {
        #region internal
        public IntPtr self;
        [DllImport("RemoteTalkClient")] static extern rtHTTPReceiver rtHTTPReceiverCreate();
        [DllImport("RemoteTalkClient")] static extern void rtHTTPReceiverRelease(IntPtr self);
        [DllImport("RemoteTalkClient")] static extern int rtHTTPReceiverConsumeAudioData(IntPtr self, rtAudioDataCallback cb);
        #endregion

        public static implicit operator bool(rtHTTPReceiver v) { return v.self != IntPtr.Zero; }

        public static rtHTTPReceiver Create() { return rtHTTPReceiverCreate(); }
        public void Release() { rtHTTPReceiverRelease(self); }
        public int Consume(rtAudioDataCallback cb) { return rtHTTPReceiverConsumeAudioData(self, cb); }
    }


    [Serializable]
    public class AvatorInfo
    {
        public int id;
        public string name;
    }
}
