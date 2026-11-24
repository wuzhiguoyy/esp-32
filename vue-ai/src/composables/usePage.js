import { reactive, ref } from 'vue'

export function usePage(fetchFn, initialQuery = {}) {
  const loading = ref(false)
  const rows = ref([])
  const total = ref(0)
  const query = reactive({ pageNum: 1, pageSize: 20, ...initialQuery })

  async function load() {
    loading.value = true
    try {
      const result = await fetchFn({ ...query })
      rows.value = result?.list || []
      total.value = result?.totalCount ?? 0
    } catch (e) {
      rows.value = []
      total.value = 0
      throw e
    } finally {
      loading.value = false
    }
  }

  function reload() {
    query.pageNum = 1
    return load()
  }

  return { loading, rows, total, query, load, reload }
}
