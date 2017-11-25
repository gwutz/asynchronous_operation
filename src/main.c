
#include <glib-object.h>
#include <gio/gio.h>

// synchronous variant
GPtrArray *
my_long_running_operation_sync ()
{
  int i, x;
  GPtrArray *ptrarray;

  x = 1;
  for (i = 0; i < 100000000; i++)
    {
      x += 1;
    }

  ptrarray = g_ptr_array_new ();
  return ptrarray;
}

// asynchronous variant
void
my_long_running_operation_cb (GTask         *task,
                              gpointer      source_object,
                              gpointer      task_data,
                              GCancellable *cancellable)
{
  int i, x;
  GPtrArray *ptrarray;

  x = 1;
  for (i = 0; i < 100000000; i++)
    {
      x += 1;
    }

  ptrarray = g_ptr_array_new ();

  g_task_return_pointer (task, ptrarray, NULL);
}

void
my_long_running_operation_async (GObject             *obj,
                                 GCancellable        *cancellable,
                                 GAsyncReadyCallback  callback,
                                 gpointer             user_data)
{
  g_autoptr (GTask) task;

  task = g_task_new (obj, cancellable, callback, user_data);

  g_task_run_in_thread (task, my_long_running_operation_cb);
}

GPtrArray *
my_long_running_operation_finish (GAsyncResult *res,
                                  GError       *error)
{
  return g_task_propagate_pointer (G_TASK (res), &error);
}

void
ready_cb (GObject      *object,
          GAsyncResult *res,
          gpointer      user_data)
{
  GPtrArray *ptr = my_long_running_operation_finish (res, NULL);
}

gint
main (gint   argc,
      gchar *argv[])
{
  gboolean sync = FALSE;

  if (sync) {
    GPtrArray *ptr = my_long_running_operation_sync ();
  } else {
    GObject *object = g_object_new (G_TYPE_OBJECT, NULL);

    my_long_running_operation_async (object, NULL, ready_cb, NULL);

    GMainLoop *loop = g_main_loop_new (NULL, FALSE);

    g_main_loop_run (loop);
  }

  return 0;
}
